std::ostream &operator<<(std::ostream &os, const TokenType &type) {
	switch(type) {
		case TokenType::ILLEGAL: os << "[ILLEGAL TOKEN]"; break;
		case TokenType::IDENT: os << "[IDENT]"; break;

		case TokenType::BOOL_TRUE: os << "true"; break;
		case TokenType::BOOL_FALSE: os << "false"; break;

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

std::ostream &operator<<(std::ostream &os, const Token &token) {
	switch(token.type) {
		case TokenType::ILLEGAL: 
			assert(false);

		case TokenType::IDENT:
		case TokenType::INT:
		case TokenType::FLOAT:
		case TokenType::HEX:
		case TokenType::OCTAL:
		case TokenType::STRING:
			os << token.value;
			break;
			
		case TokenType::BOOL_TRUE:
		case TokenType::BOOL_FALSE:
		case TokenType::BREAK:
		case TokenType::CASE:
		case TokenType::CONST:
		case TokenType::CONTINUE:
		case TokenType::DEFAULT:
		case TokenType::DEFER:
		case TokenType::ELSE:
		case TokenType::FALLTHROUGH:
		case TokenType::FOR:
		case TokenType::FUNC:
		case TokenType::PROC:
		case TokenType::IF:
		case TokenType::IMPORT:
		case TokenType::RETURN:
		case TokenType::SELECT:
		case TokenType::STRUCT:
		case TokenType::SWITCH:
		case TokenType::TYPE:
		case TokenType::VAR:
		case TokenType::SEMI:
		case TokenType::COLON:
		case TokenType::DEFINE:
		case TokenType::DOUBLE_COLON:
		case TokenType::ELLIPSE:
		case TokenType::PERIOD:
		case TokenType::COMMA:
		case TokenType::LPAREN:
		case TokenType::RPAREN:
		case TokenType::LBRACK:
		case TokenType::RBRACK:
		case TokenType::LBRACE:
		case TokenType::RBRACE:
		case TokenType::ADD:
		case TokenType::ADD_ASSIGN:
		case TokenType::INC:
		case TokenType::ARROW:
		case TokenType::SUB:
		case TokenType::SUB_ASSIGN:
		case TokenType::DEC:
		case TokenType::MUL:
		case TokenType::MUL_ASSIGN:
		case TokenType::QUO:
		case TokenType::QUO_ASSIGN:
		case TokenType::REM:
		case TokenType::REM_ASSIGN:
		case TokenType::XOR:
		case TokenType::XOR_ASSIGN:
		case TokenType::LSS:
		case TokenType::LEQ:
		case TokenType::SHL:
		case TokenType::SHL_ASSIGN:
		case TokenType::GTR:
		case TokenType::GEQ:
		case TokenType::SHR:
		case TokenType::SHR_ASSIGN:
		case TokenType::ASSIGN:
		case TokenType::EQL:
		case TokenType::NOT:
		case TokenType::NEQ:
		case TokenType::AND_NOT:
		case TokenType::AND_NOT_ASSIGN:
		case TokenType::AND:
		case TokenType::AND_ASSIGN:
		case TokenType::LAND:
		case TokenType::OR:
		case TokenType::OR_ASSIGN:
		case TokenType::LOR:
			os << token.type;
			break;

		case TokenType::END: 
			os << ""; 
			break;
	}
	return os;
}