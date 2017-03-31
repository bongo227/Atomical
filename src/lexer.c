#include "includes/lexer.h"

// Removes spaces, newlines and tabs
void clearWhitespace(Lexer *lexer) {
	while (*lexer->source == ' ' ||
		*lexer->source == '\t' ||
		(*lexer->source == '\n' && !lexer->semi) ||
		*lexer->source == '\r') {

		lexer->source++;
		lexer->column++;
	}
}

// checks if the character is a digit 0-9
bool isDigit(char *input) {
	return *input >= '0' && *input <= '9';
}

// checks if the character is a letter a-z or A-Z
bool isLetter(char *input) {
	return (*input >= 'a' && *input <= 'z') || 
		(*input >= 'A' && *input <= 'Z');
}

// returns the character as an integer
int asDigit(char *input) {
	if ('0' <= *input && *input <= '9') {
		return *input - '0';
	} else if('a' <= *input && *input <= 'f') {
		return *input - 'a' + 10;
	} else if('A' <= *input && *input <= 'F') {
		return *input - 'A' + 10;
	}

	return -1; // unrecognised digit
}

// returns the word at the start of the string
char *word(Lexer *lexer) {
	// token is an ident
	char *word = (char *)malloc(1024);

	// copy all letter characters
	char *wordPtr = word;
	int length = 0;
	do {
		*wordPtr++ = *lexer->source;
		lexer->source++;
		lexer->column++;
		length++;
	} while (isLetter(lexer->source) || isDigit(lexer->source));

	// null-terminate
	*wordPtr = '\0';
	length++;

	// shrink string to ident length
	word = realloc(word, length);

	return word;
}

// Extracts the mantiass from input into number, returning the characters written.
int extractMantissa(char **input, char **number, int base) {
	int length = 0;
	while(isDigit(*input) && asDigit(*input) < base) {
		**number = **input;
		length++;
		(*input)++;
		(*number)++;
	}

	return length;
}

// extracts the number at the start of the input string, returns error code
char *number(Lexer *lexer, TokenType *type) {
	char *number = (char *)malloc(1024 * sizeof(char));
	char *numberPtr = number;
	
	*type = INT;
	int base = 10;
	int length = 0; //amount of characters in number

	if (*lexer->source == '0') {
		lexer->source++;
		if (*lexer->source == 'x' || *lexer->source == 'X') {
			// number is hexadecimal
			*type = HEX;
			base = 16;
			lexer->source++; // skip 'x' / 'X'
			length += extractMantissa(&lexer->source, &numberPtr, base);
		} else {
			// assume number is octal
			bool octal = true;
			base = 8;
			length += extractMantissa(&lexer->source, &numberPtr, base);
			
			if (*lexer->source == '8' || *lexer->source == '9') {
				// number was not octal
				octal = false;
				base = 10;
				length += extractMantissa(&lexer->source, &numberPtr, base);
			}
			else {
				*type = OCTAL;
			}

			if (*lexer->source == '.') {
				// number is fractional
				goto fraction;
			}

			if (!octal) {
				// illegal octal number
				return "";
			}
		}
		goto exit;
	}

	base = 10;
	length += extractMantissa(&lexer->source, &numberPtr, base);

fraction:
	if (*lexer->source == '.') {
		// copy '.'
		*numberPtr++ = '.';
		lexer->source++; 
		length++;
		
		*type = FLOAT;
		base = 10;
		length += extractMantissa(&lexer->source, &numberPtr, base);
	}

exit:
	// Null terminate
	length++;
	*numberPtr = '\0';

	number = realloc(number, length * sizeof(char));
	return number;
}

char *escape(char **input, char quote) {
	char *esc = (char *)malloc(1024 * sizeof(char));
	int length = 0;
	int n = 0;
	int base = 0;
	int max = 0;

	// copy '/'
	*esc++ = '/';
	length++;
	(*input)++;

	if (**input == quote) {
		(*input)++;
	}
	 
	switch (**input) {
	case 'a':
	case 'b': 
	case 'f': 
	case 'n':
	case 'r':
	case 't':
	case 'v':
	case '\\':
		*esc++ = **input;
		length++;
		(*input)++;
		break;
	
	// octal
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		n = 3;
		base = 8;
		max = 255;
		break;

	// hex
	case 'x':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 2;
		base = 16;
		max = 255;
		break;

	// small unicode
	case 'u':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 4;
		base = 16;
		max = 0x0010FFFF;
		break;

	// full unicode
	case 'U':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 8;
		base = 16;
		max = 0x0010FFFF;
		break;

	default:
		// unkown escape sequence
		break;

	}

	int x = 0;
	while (n > 0) {
		int d = asDigit(*input);
		if (d >= base) {
			// illegal character
		}

		x = x * base + d;
		*esc++ = **input;
		length++;
		(*input)++;
		n--;
	}

	// check if unicode character is valid
	if (x > max || (0xD800 <= x && x < 0xE000)) {
		// invalid unicode code point
	}

	// null terminate
	*esc = '\0';
	length++;

	esc = realloc(esc, length * sizeof(char));
	return esc;
}

char *string(char **input) {
	char *string = (char *)malloc(1024 * sizeof(char));
	int length = 0;

	(*input)++; // skip '"'
	char *stringPtr = string;
	while (**input != '"') {
		if (**input == '\\') {
			char *esc = escape(input, '"');
			while (*esc) {
				*stringPtr++ = *esc++;
				length++;
			}
		}

		*stringPtr++ = **input;
		length++;
		(*input)++;
	}
	(*input)++; // skip '"'
	
	// null terminate
	*stringPtr = '\0';
	length++;

	string = realloc(string, length * sizeof(char));
	return string;
}

TokenType switch2(char **input, TokenType token0, TokenType token1) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	return token0;
}

TokenType switch3(char **input, TokenType token0, TokenType token1, char chr, TokenType token2) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		return token2;
	} 
	return token0;
}

TokenType switch4(char **input, TokenType token0, TokenType token1, char chr, TokenType token2, TokenType token3) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		if (**input == '=') {
			(*input)++;
			return token3;
		}	
		return token2;
	} 
	return token0;
}

// returns the second character in a string
char peek(char *input) {
	return *(++input);
}

void next(Lexer *lexer) {
	lexer->source++;
	lexer->column++;
}

// finds the token type for a word
TokenType keyword(char *word) {
	if (strcmp(word, "break") == 0) return BREAK;
	if (strcmp(word, "case") == 0) return CASE;
	if (strcmp(word, "const") == 0) return CONST;
	if (strcmp(word, "continue") == 0) return CONTINUE;
	if (strcmp(word, "default") == 0) return DEFAULT;
	if (strcmp(word, "defer") == 0) return DEFER;
	if (strcmp(word, "else") == 0) return ELSE;
	if (strcmp(word, "fallthrough") == 0) return FALLTHROUGH;
	if (strcmp(word, "for") == 0) return FOR;
	if (strcmp(word, "func") == 0) return FUNC;
	if (strcmp(word, "proc") == 0) return PROC;
	if (strcmp(word, "if") == 0) return IF;
	if (strcmp(word, "import") == 0) return IMPORT;
	if (strcmp(word, "return") == 0) return RETURN;
	if (strcmp(word, "select") == 0) return SELECT;
	if (strcmp(word, "struct") == 0) return STRUCT;
	if (strcmp(word, "switch") == 0) return SWITCH;
	if (strcmp(word, "type") == 0) return TYPE;
	if (strcmp(word, "var") == 0) return VAR;
	return IDENT;
}

Token *Lex(char *source) {
	Lexer lexer = {source, 1, 1, false};

	Token *tokens = (Token *)malloc(0);

	int i = 0;
	while (*lexer.source) {
		i++;
		clearWhitespace(&lexer);

		Token token;
		token.line = lexer.line;
		token.column = lexer.column;
		token.value = "";

		if (isLetter(lexer.source)) {
			// token is an identifier
			token.value = word(&lexer);
			token.type = keyword(token.value);
			if (token.type == IDENT || 
				token.type == BREAK || 
				token.type == CONTINUE || 
				token.type == FALLTHROUGH || 
				token.type == RETURN) {
			
				lexer.semi = true;
			}
		}
		else if (isDigit(lexer.source)) {
			// token is a number
			lexer.semi = true;
			token.type = INT;
			token.value = number(&lexer, &token.type);
		} else {
			// token is a symbol
			switch (*lexer.source) {
				case '\n':
					token.type = SEMI;
					lexer.semi = false;
					lexer.column = 1;
					lexer.line++;
					next(&lexer);
					break;

				case '"':
					lexer.semi = true;
					token.type = STRING;
					token.value = string(&lexer.source);
					break;

				case ':':
					token.type = switch3(&lexer.source, COLON, DEFINE, ':', DOUBLE_COLON);
					break;

				case '.':
					token.type = PERIOD;
					next(&lexer);
					if (*lexer.source == '.') {
						next(&lexer);
						if (*lexer.source == '.') {
							next(&lexer);
							token.type = ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = COMMA;
					next(&lexer);
					break;

				case ';':
					token.type = SEMI;
					next(&lexer);
					break;

				case '(':
					token.type = LPAREN;
					next(&lexer);
					break;

				case ')':
					lexer.semi = true;
					token.type = RPAREN;
					next(&lexer);
					break;

				case '[':
					token.type = LBRACK;
					next(&lexer);
					break;

				case ']':
					lexer.semi = true;
					token.type = RBRACK;
					next(&lexer);
					break;

				case '{':
					lexer.semi = false;
					token.type = LBRACE;
					next(&lexer);
					break;

				case '}':
					lexer.semi = true;
					token.type = RBRACE;
					next(&lexer);
					break;

				case '+':
					token.type = switch3(&lexer.source, ADD, ADD_ASSIGN, '+', INC);
					if (token.type == INC) lexer.semi = true;
					break;

				case '-':
					if (peek(lexer.source) == '>') {
						token.type = ARROW;
						lexer.source += 2;
					} else {
						token.type = switch3(&lexer.source, SUB, SUB_ASSIGN, '-', DEC);
						if (token.type == DEC) lexer.semi = true;
					}
					break;

				case '*':
					token.type = switch2(&lexer.source, MUL, MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(&lexer.source, QUO, QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(&lexer.source, REM, REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(&lexer.source, XOR, XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(&lexer.source, LSS, LEQ, '<', SHL, SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(&lexer.source, GTR, GEQ, '>', SHR, SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(&lexer.source, ASSIGN, EQL);
					break;

				case '!':
					token.type = switch2(&lexer.source, NOT, NEQ);
					break;

				case '&':
					if (peek(lexer.source) == '^') {
						next(&lexer);
						token.type = switch2(&lexer.source, AND_NOT, AND_NOT_ASSIGN);
					} else {
						token.type = switch3(&lexer.source, AND, AND_ASSIGN, '&', LAND);
					}
					break;

				case '|':
					token.type = switch3(&lexer.source, OR, OR_ASSIGN, '|', LOR);
					break;

				default:
					token.type = ILLEGAL;
					next(&lexer);
					break;
			}
		}

		if (token.value == NULL) token.value = TokenName(token.type);

		// Add the token to the array
		tokens = (Token *)realloc(tokens, (i + 1) * sizeof(Token));
		tokens[i - 1] = token;

		// Check for newline
		if (*lexer.source == '\n') {
			lexer.line++;
		}

	}

	// End of file token
	Token token;
	token.column = 321;
	token.line = 321;
	token.value = "";
	token.type = END;
	tokens[i] = token;

	return tokens;
}

char *TokenName(TokenType type) {
	switch(type) {
		case ILLEGAL: return "illegal";
		case END: return "[END]";
		case IDENT: return "[ident]";
		
		case INT: return "[int]";
		case FLOAT: return "[float]";
		case HEX: return "[hex]";
		case OCTAL: return "[octal]";
		case STRING: return "[string]";
		
		case BREAK: return "break";
		case CASE: return "case";
		case CONST: return "const";
		case CONTINUE: return "continue";
		case DEFAULT: return "default";
		case ELSE: return "else";
		case FALLTHROUGH: return "fallthrough";
		case FOR: return "for";
		case FUNC: return "func";
		case PROC: return "proc";
		case IF: return "if";
		case IMPORT: return "import";
		case RETURN: return "return";
		case SELECT: return "select";
		case STRUCT: return "struct";
		case SWITCH: return "switch";
		case TYPE: return "type";
		case VAR: return "var";
		case DEFER: return "defer";

		case DEFINE: return ":=";
		case SEMI: return ";";
		case COLON: return ":";
		case DOUBLE_COLON: return "::";
		case ELLIPSE: return "...";
		case PERIOD: return ".";
		case COMMA: return ",";
		case LPAREN: return "(";
		case RPAREN: return ")";
		case LBRACK: return "[";
		case RBRACK: return "]";
		case LBRACE: return "{";
		case RBRACE: return "}";
		case ADD: return "+";
		case ADD_ASSIGN: return "+=";
		case INC: return "++";
		case ARROW: return "->";
		case SUB: return "-";
		case SUB_ASSIGN: return "-=";
		case DEC: return "--";
		case MUL: return "*";
		case MUL_ASSIGN: return "*=";
		case QUO: return "/";
		case QUO_ASSIGN: return "/=";
		case REM: return "%";
		case REM_ASSIGN: return "%=";
		case XOR: return "^";
		case XOR_ASSIGN: return "^=";
		case GTR: return ">";
		case GEQ: return ">=";
		case LSS: return "<";
		case LEQ: return "<=";
		case SHL: return "<<";
		case SHL_ASSIGN: return "<<=";
		case SHR: return ">>";
		case SHR_ASSIGN: return ">>=";
		case ASSIGN: return "=";
		case EQL: return "==";
		case NOT: return "!";
		case NEQ: return "!=";
		case AND: return "&";
		case AND_ASSIGN: return "&=";
		case AND_NOT: return "&^";
		case AND_NOT_ASSIGN: return "&^=";
		case LAND: return "&&";
		case OR: return "|";
		case OR_ASSIGN: return "|=";
		case LOR: return "||";
	}

	return "UNKOWN_NAME";
}

char *GetLine(char *source, int line) {
	int currentLine = 1;

	while(*source != '\0') {
		if (*(source++) == '\n') currentLine++;
		
		if (currentLine == line) {
			// find the line length
			int lineLength = 0;
			while(*source && *source != '\n') {
				source++;
				lineLength++;
			}

			// copy the line to a buffer and return
			char *buff = malloc((lineLength+1) * sizeof(char));
			memcpy(buff, source - lineLength, lineLength);
			buff[lineLength] = '\0';
			return buff;
		}
	}

	return "LINE_NOT_FOUND";
}

// get_binding_power returns the left binding power of a token
int get_binding_power(TokenType type) {
	switch (type) {
	case END:
		return -10;
	// Non-binding operators
	case SEMI:
		return 0;
	// Assignment operators
	case ASSIGN:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case REM_ASSIGN:
	case OR_ASSIGN:
	case SHR_ASSIGN:
	case DEFINE:
		return 10;
	// Logical operators
	case LAND:
	case LOR:
		return 20;
	// Equality operators
	case EQL:
	case NEQ:
	case LSS:
	case GTR:
	case LEQ:
	case GEQ:
		return 30;
	// Math operators
	case ADD:
	case SUB:
		return 40;
	case MUL:
	case QUO:
	case REM:
		return 50;
	// Special unary
	case NOT:
		return 60;
	// Strongly bound
	case PERIOD:
	case LBRACK:
	case LPAREN:
		return 70;
	// Unknow token
	default:
		return 0;
	}
}