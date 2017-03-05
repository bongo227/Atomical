#include "lex.h"

void printToken(Token *token) {
	switch (token->type) {
		case IDENT:
			printf("Token {\n\tType: IDENT\n\tLine: %d\n\tColumn: %d\n\tValue: \"%s\"\n}\n", 
				token->line, token->column, token->value);
			break;

		case INT:
			printf("Token {\n\tType: INT\n\tLine: %d\n\tColumn: %d\n\tValue: \"%s\"\n}\n", 
				token->line, token->column, token->value);
			break;
		
		case FLOAT:
			printf("Token {\n\tType: FLOAT\n\tLine: %d\n\tColumn: %d\n\tValue: \"%s\"\n}\n",
				token->line, token->column, token->value);
			break;
	}
}

// Removes spaces, newlines and tabs
void clearWhitespace(Lexer *lexer) {
	while (*lexer->source == ' ' ||
		*lexer->source == '\t' ||
		(*lexer->source == '\n' && !lexer->semi) ||
		*lexer->source == '\r') {

		*lexer->source++;
		lexer->column++;
	}
}

// checks if the character is a letter a-z or A-Z
bool isLetter(char *input) {
	return *input >= 'a' && *input <= 'z' || *input >= 'A' && *input <= 'Z';
}

// checks if the character is a digit 0-9
bool isDigit(char *input) {
	return *input >= '0' && *input <= '9';
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
	} while (isLetter(lexer->source));

	// null-terminate
	*wordPtr = '\0';
	length++;

	// shrink string to ident length
	realloc(word, length);

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

	realloc(number, length * sizeof(char));
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
		n, base, max = 3, 8, 255;
		break;

	// hex
	case 'x':
		*esc++ = **input;
		length++;
		(*input)++;
		n, base, max = 2, 16, 255;
		break;

	// small unicode
	case 'u':
		*esc++ = **input;
		length++;
		(*input)++;
		n, base, max = 4, 16, 0x0010FFFF;
		break;

	// full unicode
	case 'U':
		*esc++ = **input;
		length++;
		(*input)++;
		n, base, max = 8, 16, 0x0010FFFF;
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
	if (x > max || 0xD800 <= x && x < 0xE000) {
		// invalid unicode code point
	}

	// null terminate
	*esc = '\0';
	length++;

	realloc(esc, length * sizeof(char));
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

	realloc(string, length * sizeof(char));
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
					lexer.semi = false;
					token.type = SEMI;
					next(&lexer);
					lexer.column = 1;
					lexer.line++;
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

		// Add the token to the array
		tokens = (Token *)realloc(tokens, (i + 1) * sizeof(Token));
		tokens[i - 1] = token;

		// Check for newline
		if (*lexer.source == '\n') {
			lexer.line++;
			lexer.column = 1;
			next(&lexer);
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