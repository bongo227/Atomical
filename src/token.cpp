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

std::ostream &operator<<(std::ostream &os, const TokenType &type) {
	switch(type) {
		case TokenType::ILLEGAL: os << "[ILLEGAL TOKEN]"; break;
		case TokenType::IDENT: os << "[IDENT]"; break;
		
		case TokenType::INT: os << "[INT]"; break;
		case TokenType::FLOAT: os << "[FLOAT]"; break;
		case TokenType::HEX: os << "[HEX]"; break;
		case TokenType::OCTAL: os << "[OCTAL]"; break;
		case TokenType::STRING: os << "[STRING]"; break;
		
		case TokenType::BREAK: os << "break"; break;
		case TokenType::CASE: os << "case"; break;
		case TokenType::CONST: os << "const"; break;
		case TokenType::CONTINUE: os << "continue"; break;
		case TokenType::DEFAULT: os << "default"; break;
		case TokenType::DEFER: os << "defer"; break;
		case TokenType::ELSE: os << "else"; break;
		case TokenType::FALLTHROUGH: os << "fallthrough"; break;
		case TokenType::FOR: os << "for"; break;
		case TokenType::FUNC: os << "func"; break;
		case TokenType::PROC: os << "proc"; break;
		case TokenType::IF: os << "if"; break;
		case TokenType::IMPORT: os << "import"; break;
		case TokenType::RETURN: os << "return"; break;
		case TokenType::SELECT: os << "select"; break;
		case TokenType::STRUCT: os << "struct"; break;
		case TokenType::SWITCH: os << "switch"; break;
		case TokenType::TYPE: os << "type"; break;
		case TokenType::VAR: os << "var"; break;

		case TokenType::SEMI: os << ";"; break;
		case TokenType::COLON: os << ":"; break;
		case TokenType::DEFINE: os << ":="; break;
		case TokenType::DOUBLE_COLON: os << "::"; break;
		case TokenType::ELLIPSE: os << "..."; break;
		case TokenType::PERIOD: os << "."; break;
		case TokenType::COMMA: os << ","; break;
		case TokenType::LPAREN: os << "("; break;
		case TokenType::RPAREN: os << ")"; break;
		case TokenType::LBRACK: os << "["; break;
		case TokenType::RBRACK: os << "]"; break;
		case TokenType::LBRACE: os << "{"; break;
		case TokenType::RBRACE: os << "}"; break;
		case TokenType::ADD: os << "+"; break;
		case TokenType::ADD_ASSIGN: os << "+="; break;
		case TokenType::INC: os << "++"; break;
		case TokenType::ARROW: os << "->"; break;
		case TokenType::SUB: os << "-"; break;
		case TokenType::SUB_ASSIGN: os << "-="; break;
		case TokenType::DEC: os << "--"; break;
		case TokenType::MUL: os << "*"; break;
		case TokenType::MUL_ASSIGN: os << "*="; break;
		case TokenType::QUO: os << "/"; break;
		case TokenType::QUO_ASSIGN: os << "/="; break;
		case TokenType::REM: os << "%"; break;
		case TokenType::REM_ASSIGN: os << "%="; break;
		case TokenType::XOR: os << "^"; break;
		case TokenType::XOR_ASSIGN: os << "^="; break;
		case TokenType::LSS: os << "<"; break;
		case TokenType::LEQ: os << "<="; break;
		case TokenType::SHL: os << "<<"; break;
		case TokenType::SHL_ASSIGN: os << "<<="; break;
		case TokenType::GTR: os << ">"; break;
		case TokenType::GEQ: os << ">="; break;
		case TokenType::SHR: os << ">>"; break;
		case TokenType::SHR_ASSIGN: os << ">>="; break;
		case TokenType::ASSIGN: os << "="; break;
		case TokenType::EQL: os << "=="; break;
		case TokenType::NOT: os << "!"; break;
		case TokenType::NEQ: os << "!="; break;
		case TokenType::AND_NOT: os << "&^"; break;
		case TokenType::AND_NOT_ASSIGN: os << "&^="; break;
		case TokenType::AND: os << "&"; break;
		case TokenType::AND_ASSIGN: os << "&="; break;
		case TokenType::LAND: os << "&&"; break;
		case TokenType::OR: os << "|"; break;
		case TokenType::OR_ASSIGN: os << "|="; break;
		case TokenType::LOR: os << "||"; break;

		case TokenType::END: os << "[END]"; break;
	}
	return os;
}

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