#include <string>
#include <assert.h>

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

    public:
        int get_binding_power();
};

int Token::get_binding_power() {
    switch(type) {
        case SEMI:
            return 0;

        // Assign / define
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

        // Unknown token
        default:
            assert(false);
            return 0;
    }
}