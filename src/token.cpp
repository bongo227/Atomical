#include <string>
#include <assert.h>

enum class TokenType {
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

    public:
        int get_binding_power();
};

int Token::get_binding_power() {
    switch(type) {
        // Assign / define
        case TokenType::ASSIGN:
        case TokenType::ADD_ASSIGN:
        case TokenType::SUB_ASSIGN:
        case TokenType::MUL_ASSIGN:
        case TokenType::REM_ASSIGN:
        case TokenType::OR_ASSIGN:
        case TokenType::SHR_ASSIGN:
        case TokenType::DEFINE:
            return 0; // if things break change to '10'

        // Logical operators
        case TokenType::LAND:
        case TokenType::LOR:
            return 20;
            
        // Equality operators
        case TokenType::EQL:
        case TokenType::NEQ:
        case TokenType::LSS:
        case TokenType::GTR:
        case TokenType::LEQ:
        case TokenType::GEQ:
            return 30;

        // Math operators
        case TokenType::ADD:
        case TokenType::SUB:
            return 40;

        case TokenType::MUL:
        case TokenType::QUO:
        case TokenType::REM:
            return 50;

        // Special unary
        case TokenType::NOT:
            return 60;

        // Strongly bound
        case TokenType::PERIOD:
        case TokenType::LBRACK:
        case TokenType::LPAREN:
            return 70;

        // Unknown token
        default:
            return 0;
    }
}