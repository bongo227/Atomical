#include <string>
#include <vector>
#include <tuple>
#include <deque>

class Lexer {
    private:
        std::string source;
        size_t index;
        int line;
        int column;
        bool semi;

    public:
        explicit Lexer(std::string source);
        std::deque<Token> lex();

    private:
        void clear_whitespace();
        bool is_digit();
        bool is_letter();
        int as_digit(char c);
        std::string next_mantissa(int base);
        std::tuple<TokenType, std::string> next_number();
        std::tuple<TokenType, std::string> next_ident();
        std::string next_escape(char quote);
        std::string next_string();

        TokenType switch2(TokenType t0, TokenType t1);
        TokenType switch3(TokenType t0, TokenType t1, char c, TokenType t2);
        TokenType switch4(TokenType t0, TokenType t1, char c, TokenType t2, TokenType t3);
};

Lexer::Lexer(std::string source) : source(source), index(0), line(1), column(1), semi(false) {}

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

std::string Lexer::next_mantissa(int base) {
    std::string number = "";
    while(is_digit() && this->as_digit(source[index]) < base) {
        number += source[index++];
    }

    return number;
}

std::tuple<TokenType, std::string> Lexer::next_number() {
    std::string number = "";
    TokenType type = TokenType::INT;
    int base = 10;    

    if (source[index] == '0') {
        index++;
        if(source[index] == 'x' || source[index] == 'X') {
            type = TokenType::HEX;
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
                type = TokenType::OCTAL;
            }

            if (source[index] == '.') goto fraction;
            if (!octal) return std::make_tuple(TokenType::ILLEGAL, ""); // Illegal number
        }
        goto exit;
    }

    base = 10;
    number += next_mantissa(base);

fraction:
    if (source[index] == '.') {
        number += source[index++];
        type = TokenType::FLOAT;
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

std::tuple<TokenType, std::string> Lexer::next_ident() {
    if(!is_letter()) {
        return std::make_tuple(TokenType::ILLEGAL, "");
    }

    std::string ident = "";
    do {
        ident += source[index++];
    } while(is_letter() || is_digit());

    TokenType type = TokenType::IDENT;
    if(ident == "break") type = TokenType::BREAK;
    else if(ident == "case") type = TokenType::CASE;
    else if(ident == "const") type = TokenType::CONST;
    else if(ident == "continue") type = TokenType::CONTINUE;
    else if(ident == "default") type = TokenType::DEFAULT;
    else if(ident == "defer") type = TokenType::DEFER;
    else if(ident == "else") type = TokenType::ELSE;
    else if(ident == "fallthrough") type = TokenType::FALLTHROUGH;
    else if(ident == "for") type = TokenType::FOR;
    else if(ident == "func") type = TokenType::FUNC;
    else if(ident == "proc") type = TokenType::PROC;
    else if(ident == "if") type = TokenType::IF;
    else if(ident == "import") type = TokenType::IMPORT;
    else if(ident == "return") type = TokenType::RETURN;
    else if(ident == "select") type = TokenType::SELECT;
    else if(ident == "struct") type = TokenType::STRUCT;
    else if(ident == "switch") type = TokenType::SWITCH;
    else if(ident == "type") type = TokenType::TYPE;
    else if(ident == "var") type = TokenType::VAR;

    return std::make_tuple(type, ident);
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
    if (source[index+1] == '=') {
        index++;
        return t1;
    } else {
        return t0;
    }
}

TokenType Lexer::switch3(TokenType t0, TokenType t1, char c, TokenType t2) {
    if(source[index+1] == '=') {
        index++;
        return t1;
    } else if (source[index+1] == c) {
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

std::deque<Token> Lexer::lex() {
    std::deque<Token> tokens;

    clear_whitespace();
    while (source.length() > index) {
        Token token = {TokenType::ILLEGAL, line, column, ""};

        if (is_letter()) {
            auto ident = next_ident();
            token.type = std::get<0>(ident);
            token.value = std::get<1>(ident); 
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
                    token.type = TokenType::SEMI;
                    column = 1;
                    line++;
                    semi = true;
                    break;
                case '"':
                    token.type = TokenType::STRING;
                    token.value = next_string();
                    semi = true;
                    break;
                
                case ':':
					token.type = switch3(TokenType::COLON, TokenType::DEFINE, ':', TokenType::DOUBLE_COLON);
					break;

				case '.':
					token.type = TokenType::PERIOD;
					index++;
					if (source[index] == '.') {
						index++;
						if (source[index] == '.') {
							token.type = TokenType::ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = TokenType::COMMA;
					break;

				case ';':
					token.type = TokenType::SEMI;
					break;

				case '(':
					token.type = TokenType::LPAREN;
					break;

				case ')':
					semi = true;
					token.type = TokenType::RPAREN;
					break;

				case '[':
					token.type = TokenType::LBRACK;
					break;

				case ']':
					semi = true;
					token.type = TokenType::RBRACK;
					break;

				case '{':
					semi = false;
					token.type = TokenType::LBRACE;
					break;

				case '}':
					semi = true;
					token.type = TokenType::RBRACE;
					break;

				case '+':
					token.type = switch3(TokenType::ADD, TokenType::ADD_ASSIGN, '+', TokenType::INC);
					if (token.type == TokenType::INC) semi = true;
					break;

				case '-':
					if (source[index+1] == '>') {
						token.type = TokenType::ARROW;
						index++;
					} else {
						token.type = switch3(TokenType::SUB, TokenType::SUB_ASSIGN, '-', TokenType::DEC);
						if (token.type == TokenType::DEC) semi = true;
					}
					break;

				case '*':
					token.type = switch2(TokenType::MUL, TokenType::MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(TokenType::QUO, TokenType::QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(TokenType::REM, TokenType::REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(TokenType::XOR, TokenType::XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(TokenType::LSS, TokenType::LEQ, '<', TokenType::SHL, TokenType::SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(TokenType::GTR, TokenType::GEQ, '>', TokenType::SHR, TokenType::SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(TokenType::ASSIGN, TokenType::EQL);
					break;

				case '!':
					token.type = switch2(TokenType::NOT, TokenType::NEQ);
					break;

				case '&':
					if (source[index+1] == '^') {
						index++;
						token.type = switch2(TokenType::AND_NOT, TokenType::AND_NOT_ASSIGN);
					} else {
						token.type = switch3(TokenType::AND, TokenType::AND_ASSIGN, '&', TokenType::LAND);
					}
					break;

				case '|':
					token.type = switch3(TokenType::OR, TokenType::OR_ASSIGN, '|', TokenType::LOR);
					break;
            }

            index++;
        }

        tokens.push_back(token);
        clear_whitespace();
    }

    return tokens;
}