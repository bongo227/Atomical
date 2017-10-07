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
        std::tuple<enum TokenType, std::string> next_number();
        std::tuple<enum TokenType, std::string> next_ident();
        std::string next_escape(char quote);
        std::string next_string();

        enum TokenType switch2(enum TokenType t0, enum TokenType t1);
        enum TokenType switch3(enum TokenType t0, enum TokenType t1, char c, enum TokenType t2);
        enum TokenType switch4(enum TokenType t0, enum TokenType t1, char c, enum TokenType t2, enum TokenType t3);
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

std::tuple<enum TokenType, std::string> Lexer::next_number() {
    std::string number = "";
    enum TokenType type = TokenType::LITERAL_INT;
    int base = 10;    

    if (source[index] == '0') {
        index++;
        if(source[index] == 'x' || source[index] == 'X') {
            type = TokenType::LITERAL_HEX;
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
                type = TokenType::LITERAL_OCTAL;
            }

            if (source[index] == '.') goto fraction;
            if (!octal) return std::make_tuple(TokenType::TOKEN_ILLEGAL, ""); // Illegal number
        }
        goto exit;
    }

    base = 10;
    number += next_mantissa(base);

fraction:
    if (source[index] == '.') {
        number += source[index++];
        type = TokenType::LITERAL_FLOAT;
        base = 10;
        number += next_mantissa(base);
    }
exit:
    if(type == TokenType::LITERAL_OCTAL && number == "") {
        type = TokenType::LITERAL_INT;
        number = "0";
    }
    return std::make_tuple(type, number);

    return std::make_tuple(TokenType::TOKEN_ILLEGAL, "");
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

std::tuple<enum TokenType, std::string> Lexer::next_ident() {
    if(!is_letter()) {
        return std::make_tuple(TokenType::TOKEN_ILLEGAL, "");
    }

    std::string ident = "";
    do {
        ident += source[index++];
    } while(is_letter() || is_digit());

    enum TokenType type = TokenType::TOKEN_IDENT;
    if(ident == "break") type = TokenType::KEY_WORD_BREAK;
    else if(ident == "case") type = TokenType::KEY_WORD_CASE;
    else if(ident == "const") type = TokenType::KEY_WORD_CONST;
    else if(ident == "continue") type = TokenType::KEY_WORD_CONTINUE;
    else if(ident == "default") type = TokenType::KEY_WORD_DEFAULT;
    else if(ident == "defer") type = TokenType::KEY_WORD_DEFER;
    else if(ident == "else") type = TokenType::KEY_WORD_ELSE;
    else if(ident == "fallthrough") type = TokenType::KEY_WORD_FALLTHROUGH;
    else if(ident == "for") type = TokenType::KEY_WORD_FOR;
    else if(ident == "func") type = TokenType::KEY_WORD_FUNC;
    else if(ident == "proc") type = TokenType::KEY_WORD_PROC;
    else if(ident == "if") type = TokenType::KEY_WORD_IF;
    else if(ident == "import") type = TokenType::KEY_WORD_IMPORT;
    else if(ident == "return") type = TokenType::KEY_WORD_RETURN;
    else if(ident == "select") type = TokenType::KEY_WORD_SELECT;
    else if(ident == "struct") type = TokenType::KEY_WORD_STRUCT;
    else if(ident == "switch") type = TokenType::KEY_WORD_SWITCH;
    else if(ident == "type") type = TokenType::KEY_WORD_TYPE;
    else if(ident == "var") type = TokenType::KEY_WORD_VAR;
    else if(ident == "true") type = TokenType::BOOL_TRUE;
    else if(ident == "false") type = TokenType::BOOL_FALSE;

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

enum TokenType Lexer::switch2(enum TokenType t0, enum TokenType t1) {
    if (source[index+1] == '=') {
        index++;
        return t1;
    } else {
        return t0;
    }
}

enum TokenType Lexer::switch3(enum TokenType t0, enum TokenType t1, char c, enum TokenType t2) {
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

enum TokenType Lexer::switch4(enum TokenType t0, enum TokenType t1, char c, enum TokenType t2, enum TokenType t3) {
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
        Token token = {TokenType::TOKEN_ILLEGAL, line, column, ""};

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
                    token.type = TokenType::SYMBOL_SEMI;
                    column = 1;
                    line++;
                    semi = false;
                    break;
                case '"':
                    token.type = TokenType::LITERAL_STRING;
                    token.value = next_string();
                    semi = true;
                    break;
                
                case ':':
					token.type = switch3(TokenType::SYMBOL_COLON, TokenType::SYMBOL_DEFINE, ':', TokenType::SYMBOL_DOUBLE_COLON);
					break;

				case '.':
					token.type = TokenType::SYMBOL_PERIOD;
					index++;
					if (source[index] == '.') {
						index++;
						if (source[index] == '.') {
							token.type = TokenType::SYMBOL_ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = TokenType::SYMBOL_COMMA;
					break;

				case ';':
					token.type = TokenType::SYMBOL_SEMI;
					break;

				case '(':
					token.type = TokenType::SYMBOL_LPAREN;
					break;

				case ')':
					semi = true;
					token.type = TokenType::SYMBOL_RPAREN;
					break;

				case '[':
					token.type = TokenType::SYMBOL_LBRACK;
					break;

				case ']':
					semi = true;
					token.type = TokenType::SYMBOL_RBRACK;
					break;

				case '{':
					semi = false;
					token.type = TokenType::SYMBOL_LBRACE;
					break;

				case '}':
                    semi = true;

                    // Insert semicolon for last statement in a block
                    if (tokens.size() && 
                        tokens.back().type != TokenType::SYMBOL_SEMI &&
                        tokens.back().type != TokenType::SYMBOL_LBRACE) {
                        
                            token.type = TokenType::SYMBOL_SEMI;
                        column = 1;
                        line++;
                        tokens.push_back(token);
                    }
                    
                    token = {TokenType::TOKEN_ILLEGAL, line, column, ""};
                    token.type = TokenType::SYMBOL_RBRACE;
					break;

				case '+':
					token.type = switch3(TokenType::SYMBOL_ADD, TokenType::SYMBOL_ADD_ASSIGN, '+', TokenType::SYMBOL_INC);
					if (token.type == TokenType::SYMBOL_INC) semi = true;
					break;

				case '-':
					if (source[index+1] == '>') {
						token.type = TokenType::SYMBOL_ARROW;
						index++;
					} else {
						token.type = switch3(TokenType::SYMBOL_SUB, TokenType::SYMBOL_SUB_ASSIGN, '-', TokenType::SYMBOL_DEC);
						if (token.type == TokenType::SYMBOL_DEC) semi = true;
					}
					break;

				case '*':
					token.type = switch2(TokenType::SYMBOL_MUL, TokenType::SYMBOL_MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(TokenType::SYMBOL_QUO, TokenType::SYMBOL_QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(TokenType::SYMBOL_REM, TokenType::SYMBOL_REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(TokenType::SYMBOL_XOR, TokenType::SYMBOL_XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(TokenType::SYMBOL_LSS, TokenType::SYMBOL_LEQ, '<', TokenType::SYMBOL_SHL, TokenType::SYMBOL_SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(TokenType::SYMBOL_GTR, TokenType::SYMBOL_GEQ, '>', TokenType::SYMBOL_SHR, TokenType::SYMBOL_SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(TokenType::SYMBOL_ASSIGN, TokenType::SYMBOL_EQL);
					break;

				case '!':
					token.type = switch2(TokenType::SYMBOL_NOT, TokenType::SYMBOL_NEQ);
					break;

				case '&':
					if (source[index+1] == '^') {
						index++;
						token.type = switch2(TokenType::SYMBOL_AND_NOT, TokenType::SYMBOL_AND_NOT_ASSIGN);
					} else {
						token.type = switch3(TokenType::SYMBOL_AND, TokenType::SYMBOL_AND_ASSIGN, '&', TokenType::SYMBOL_LAND);
					}
					break;

				case '|':
					token.type = switch3(TokenType::SYMBOL_OR, TokenType::SYMBOL_OR_ASSIGN, '|', TokenType::SYMBOL_LOR);
					break;
            }

            index++;
        }

        tokens.push_back(token);
        clear_whitespace();
    }

    if (semi && tokens.back().type != TokenType::SYMBOL_SEMI)
        tokens.push_back({TokenType::SYMBOL_SEMI, line, column, ""});
    

    return tokens;
}