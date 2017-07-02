#include <string>
#include <vector>
#include <tuple>

enum TokenType {
    ILLEGAL,
	IDENT,
	
	// literals
	INT,
	FLOAT,
	HEX,
	OCTAL,
	STRING,

	// key words
	BREAK,
	CASE,
	CONST,
	CONTINUE,
	DEFAULT,
	DEFER,
	ELSE,
	FALLTHROUGH,
	FOR,
	FUNC,
	PROC,
	IF,
	IMPORT,
	RETURN,
	SELECT,
	STRUCT,
	SWITCH,
	TYPE,
	VAR,

	// symbols
	SEMI,
	COLON,
	DEFINE,
	DOUBLE_COLON,
	ELLIPSE,
	PERIOD,
	COMMA,
	LPAREN,
	RPAREN,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	ADD,
	ADD_ASSIGN,
	INC,
	ARROW,
	SUB,
	SUB_ASSIGN,
	DEC,
	MUL,
	MUL_ASSIGN,
	QUO,
	QUO_ASSIGN,
	REM,
	REM_ASSIGN,
	XOR,
	XOR_ASSIGN,
	LSS,
	LEQ,
	SHL,
	SHL_ASSIGN,
	GTR,
	GEQ,
	SHR,
	SHR_ASSIGN,
	ASSIGN,
	EQL,
	NOT,
	NEQ,
	AND_NOT,
	AND_NOT_ASSIGN,
	AND,
	AND_ASSIGN,
	LAND,
	OR,
	OR_ASSIGN,
	LOR,
	
	// eof token
	END,
};

struct Token {
    TokenType type;
    int line;
    int column;
    std::string value;
};

class Lexer {
    private:
        std::string source;
        int index;
        int line;
        int column;
        bool semi;

    public:
        Lexer(std::string source);
        std::vector<Token> lex();

    private:
        void clear_whitespace();
        bool is_digit();
        bool is_letter();
        int as_digit(char c);
        std::string next_ident();
        std::string next_mantissa(int base);
        std::tuple<TokenType, std::string> next_number();
        std::string next_escape(char quote);
        std::string next_string();

        TokenType switch2(TokenType t0, TokenType t1);
        TokenType switch3(TokenType t0, TokenType t1, char c, TokenType t2);
        TokenType switch4(TokenType t0, TokenType t1, char c, TokenType t2, TokenType t3);
};

Lexer::Lexer(std::string src) {
    source = src;
    index = 0;
    line = 1;
    column = 1;
    semi = false;
}

void Lexer::clear_whitespace() {
    while(source[index] == ' ' || 
        source[index] == '\t' || 
        (source[index] == '\n' && !semi) ||
        source[index] == '\r') {
        
        index++;
        column++;
    }
}

bool Lexer::is_digit() {
    return source[index] >= '0' && source[index] <= '9';
}

bool Lexer::is_letter() {
    return (source[index] >= 'a' && source[index] <= 'z') || 
        (source[index] >= 'A' && source[index] <= 'Z');
}

int Lexer::as_digit(char c) {
    if(is_digit()) {
        return c - '0';
    } else if('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return -1;
    }
}

std::string Lexer::next_ident() {
    if(!is_letter()) {
        return "";
    }

    std::string ident = "";
    do {
        ident += source[index++];
    } while(is_letter() || is_digit());

    return ident;
}

std::string Lexer::next_mantissa(int base) {
    std::string number = "";
    while(is_digit() && this->as_digit(source[index]) < base) {
        number += source[index++];
    }

    return number;
}

std::tuple<TokenType, std::string> Lexer::next_number() {
    std::string number = "";
    TokenType type = INT;
    int base = 10;    

    if (source[index] == '0') {
        index++;
        if(source[index] == 'x' || source[index] == 'X') {
            type = HEX;
            base = 16;
            index++;
            number += next_mantissa(base);
        } else {
            // Assume octal
            bool octal = true;
            base = 8;
            number += next_mantissa(base);

            if(source[index] == '8' || source[index] == '9') {
                octal = false;
                base = 10;
                number += next_mantissa(base);
            } else {
                type = OCTAL;
            }

            if (source[index] == '.') goto fraction;
            if (!octal) return std::make_tuple(ILLEGAL, ""); // Illegal number
        }
        goto exit;
    }

    base = 10;
    number += next_mantissa(base);

fraction:
    if (source[index] == '.') {
        number += source[index++];
        type = FLOAT;
        base = 10;
        number += next_mantissa(base);
    }
exit:
    return std::make_tuple(type, number);

    return std::make_tuple(TokenType::ILLEGAL, "");
}

std::string Lexer::next_escape(char quote) {
    std::string escape = "";
    int n = 0;
    int base = 0;
    int max = 0;

    escape += source[index++];
    if (source[index] == quote) index++;

    switch (source[index]) {
        case 'a':
        case 'b': 
        case 'f': 
        case 'n':
        case 'r':
        case 't':
        case 'v':
        case '\\':
            escape += source[index++];
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
            escape += source[index++];
            n = 2;
            base = 16;
            max = 255;
            break;

        // small unicode
        case 'u':
            escape += source[index++];
            n = 4;
            base = 16;
            max = 0x0010FFFF;
            break;

        // full unicode
        case 'U':
            escape += source[index++];
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
        int d = as_digit(source[index]);
        if (d >= base) {
            // illegal character
            return "";
        }

        x = x * base + d;
        escape += source[index++];
        n--;
    }

    // check if unicode character is valid
	if (x > max || (0xD800 <= x && x < 0xE000)) {
		// invalid unicode code point
        return "";
	}

    return escape;
}

std::string Lexer::next_string() {
    std::string string = "";
    
    source[index++];
    while(source[index] != '"') {
        if(source[index] == '\\') string += next_escape('"');
        string += source[index++];
    }

    return string;
}

TokenType Lexer::switch2(TokenType t0, TokenType t1) {
    index++;
    if (source[index] == '=') {
        index++;
        return t1;
    } else {
        return t0;
    }
}

TokenType Lexer::switch3(TokenType t0, TokenType t1, char c, TokenType t2) {
    index++;
    if(source[index] == '=') {
        index++;
        return t1;
    } else if (source[index] == c) {
        index++;
        return t2;
    } else {
        return t0;
    }
}

TokenType Lexer::switch4(TokenType t0, TokenType t1, char c, TokenType t2, TokenType t3) {
    index++;
    if(source[index] == '=') {
        index++;
        return t1;
    } else if (source[index] == c) {
        index++;
        if(source[index] == '=') {
            index++;
            return t3;
        }
        return t2;
    } else {
        return t0;
    }
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens(0);

    clear_whitespace();
    while (source.length() > index) {
        Token token = {ILLEGAL, line, column, ""};

        if (is_letter()) {
            token.type = TokenType::IDENT;
            token.value = next_ident();
            column += token.value.length();
            semi = true;
        } else if (is_digit()) {
            auto number = next_number();
            token.type = std::get<0>(number);
            token.value = std::get<1>(number);
            column += token.value.length();
            semi = true;
        } else {
            switch(source[index]) {
                case '\n':
                    token.type = SEMI;
                    column = 1;
                    line++;
                    semi = true;
                    break;
                case '"':
                    token.type = STRING;
                    token.value = next_string();
                    semi = true;
                    break;
                
                case ':':
					token.type = switch3(COLON, DEFINE, ':', DOUBLE_COLON);
					break;

				case '.':
					token.type = PERIOD;
					index++;
					if (source[index] == '.') {
						index++;
						if (source[index] == '.') {
							token.type = ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = COMMA;
					break;

				case ';':
					token.type = SEMI;
					break;

				case '(':
					token.type = LPAREN;
					break;

				case ')':
					semi = true;
					token.type = RPAREN;
					break;

				case '[':
					token.type = LBRACK;
					break;

				case ']':
					semi = true;
					token.type = RBRACK;
					break;

				case '{':
					semi = false;
					token.type = LBRACE;
					break;

				case '}':
					semi = true;
					token.type = RBRACE;
					break;

				case '+':
					token.type = switch3(ADD, ADD_ASSIGN, '+', INC);
					if (token.type == INC) semi = true;
					break;

				case '-':
					if (source[index+1] == '>') {
						token.type = ARROW;
						index++;
					} else {
						token.type = switch3(SUB, SUB_ASSIGN, '-', DEC);
						if (token.type == DEC) semi = true;
					}
					break;

				case '*':
					token.type = switch2(MUL, MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(QUO, QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(REM, REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(XOR, XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(LSS, LEQ, '<', SHL, SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(GTR, GEQ, '>', SHR, SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(ASSIGN, EQL);
					break;

				case '!':
					token.type = switch2(NOT, NEQ);
					break;

				case '&':
					if (source[index+1] == '^') {
						index++;
						token.type = switch2(AND_NOT, AND_NOT_ASSIGN);
					} else {
						token.type = switch3(AND, AND_ASSIGN, '&', LAND);
					}
					break;

				case '|':
					token.type = switch3(OR, OR_ASSIGN, '|', LOR);
					break;
            }

            index++;
        }

        tokens.push_back(token);
        clear_whitespace();
    }

    return tokens;
}