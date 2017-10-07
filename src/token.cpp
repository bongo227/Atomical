std::ostream &operator<<(std::ostream &os, const enum TokenType &type) {
	switch(type) {
		case TokenType::TOKEN_ILLEGAL: os << "[ILLEGAL TOKEN]"; break;
		case TokenType::TOKEN_IDENT: os << "[IDENT]"; break;

		case TokenType::BOOL_TRUE: os << "true"; break;
		case TokenType::BOOL_FALSE: os << "false"; break;

		case TokenType::LITERAL_INT: os << "[INT]"; break;
		case TokenType::LITERAL_FLOAT: os << "[FLOAT]"; break;
		case TokenType::LITERAL_HEX: os << "[HEX]"; break;
		case TokenType::LITERAL_OCTAL: os << "[OCTAL]"; break;
		case TokenType::LITERAL_STRING: os << "[STRING]"; break;
		
		case TokenType::KEY_WORD_BREAK: os << "break"; break;
		case TokenType::KEY_WORD_CASE: os << "case"; break;
		case TokenType::KEY_WORD_CONST: os << "const"; break;
		case TokenType::KEY_WORD_CONTINUE: os << "continue"; break;
		case TokenType::KEY_WORD_DEFAULT: os << "default"; break;
		case TokenType::KEY_WORD_DEFER: os << "defer"; break;
		case TokenType::KEY_WORD_ELSE: os << "else"; break;
		case TokenType::KEY_WORD_FALLTHROUGH: os << "fallthrough"; break;
		case TokenType::KEY_WORD_FOR: os << "for"; break;
		case TokenType::KEY_WORD_FUNC: os << "func"; break;
		case TokenType::KEY_WORD_PROC: os << "proc"; break;
		case TokenType::KEY_WORD_IF: os << "if"; break;
		case TokenType::KEY_WORD_IMPORT: os << "import"; break;
		case TokenType::KEY_WORD_RETURN: os << "return"; break;
		case TokenType::KEY_WORD_SELECT: os << "select"; break;
		case TokenType::KEY_WORD_STRUCT: os << "struct"; break;
		case TokenType::KEY_WORD_SWITCH: os << "switch"; break;
		case TokenType::KEY_WORD_TYPE: os << "type"; break;
		case TokenType::KEY_WORD_VAR: os << "var"; break;

		case TokenType::SYMBOL_SEMI: os << ";"; break;
		case TokenType::SYMBOL_COLON: os << ":"; break;
		case TokenType::SYMBOL_DEFINE: os << ":="; break;
		case TokenType::SYMBOL_DOUBLE_COLON: os << "::"; break;
		case TokenType::SYMBOL_ELLIPSE: os << "..."; break;
		case TokenType::SYMBOL_PERIOD: os << "."; break;
		case TokenType::SYMBOL_COMMA: os << ","; break;
		case TokenType::SYMBOL_LPAREN: os << "("; break;
		case TokenType::SYMBOL_RPAREN: os << ")"; break;
		case TokenType::SYMBOL_LBRACK: os << "["; break;
		case TokenType::SYMBOL_RBRACK: os << "]"; break;
		case TokenType::SYMBOL_LBRACE: os << "{"; break;
		case TokenType::SYMBOL_RBRACE: os << "}"; break;
		case TokenType::SYMBOL_ADD: os << "+"; break;
		case TokenType::SYMBOL_ADD_ASSIGN: os << "+="; break;
		case TokenType::SYMBOL_INC: os << "++"; break;
		case TokenType::SYMBOL_ARROW: os << "->"; break;
		case TokenType::SYMBOL_SUB: os << "-"; break;
		case TokenType::SYMBOL_SUB_ASSIGN: os << "-="; break;
		case TokenType::SYMBOL_DEC: os << "--"; break;
		case TokenType::SYMBOL_MUL: os << "*"; break;
		case TokenType::SYMBOL_MUL_ASSIGN: os << "*="; break;
		case TokenType::SYMBOL_QUO: os << "/"; break;
		case TokenType::SYMBOL_QUO_ASSIGN: os << "/="; break;
		case TokenType::SYMBOL_REM: os << "%"; break;
		case TokenType::SYMBOL_REM_ASSIGN: os << "%="; break;
		case TokenType::SYMBOL_XOR: os << "^"; break;
		case TokenType::SYMBOL_XOR_ASSIGN: os << "^="; break;
		case TokenType::SYMBOL_LSS: os << "<"; break;
		case TokenType::SYMBOL_LEQ: os << "<="; break;
		case TokenType::SYMBOL_SHL: os << "<<"; break;
		case TokenType::SYMBOL_SHL_ASSIGN: os << "<<="; break;
		case TokenType::SYMBOL_GTR: os << ">"; break;
		case TokenType::SYMBOL_GEQ: os << ">="; break;
		case TokenType::SYMBOL_SHR: os << ">>"; break;
		case TokenType::SYMBOL_SHR_ASSIGN: os << ">>="; break;
		case TokenType::SYMBOL_ASSIGN: os << "="; break;
		case TokenType::SYMBOL_EQL: os << "=="; break;
		case TokenType::SYMBOL_NOT: os << "!"; break;
		case TokenType::SYMBOL_NEQ: os << "!="; break;
		case TokenType::SYMBOL_AND_NOT: os << "&^"; break;
		case TokenType::SYMBOL_AND_NOT_ASSIGN: os << "&^="; break;
		case TokenType::SYMBOL_AND: os << "&"; break;
		case TokenType::SYMBOL_AND_ASSIGN: os << "&="; break;
		case TokenType::SYMBOL_LAND: os << "&&"; break;
		case TokenType::SYMBOL_OR: os << "|"; break;
		case TokenType::SYMBOL_OR_ASSIGN: os << "|="; break;
		case TokenType::SYMBOL_LOR: os << "||"; break;

		case TokenType::TOKEN_END: os << "[END]"; break;
	}
	return os;
}

int Token::get_binding_power() {
    switch(type) {
        // Assign / define
        case TokenType::SYMBOL_ASSIGN:
        case TokenType::SYMBOL_ADD_ASSIGN:
        case TokenType::SYMBOL_SUB_ASSIGN:
        case TokenType::SYMBOL_MUL_ASSIGN:
        case TokenType::SYMBOL_REM_ASSIGN:
        case TokenType::SYMBOL_OR_ASSIGN:
        case TokenType::SYMBOL_SHR_ASSIGN:
        case TokenType::SYMBOL_DEFINE:
            return 0; // if things break change to '10'

        // Logical operators
        case TokenType::SYMBOL_LAND:
        case TokenType::SYMBOL_LOR:
            return 20;
            
        // Equality operators
        case TokenType::SYMBOL_EQL:
        case TokenType::SYMBOL_NEQ:
        case TokenType::SYMBOL_LSS:
        case TokenType::SYMBOL_GTR:
        case TokenType::SYMBOL_LEQ:
        case TokenType::SYMBOL_GEQ:
            return 30;

        // Math operators
        case TokenType::SYMBOL_ADD:
        case TokenType::SYMBOL_SUB:
            return 40;

        case TokenType::SYMBOL_MUL:
        case TokenType::SYMBOL_QUO:
        case TokenType::SYMBOL_REM:
            return 50;

        // Special unary
        case TokenType::SYMBOL_NOT:
            return 60;

        // Strongly bound
        case TokenType::SYMBOL_PERIOD:
        case TokenType::SYMBOL_LBRACK:
        case TokenType::SYMBOL_LPAREN:
            return 70;

        // Unknown token
        default:
            return 0;
    }
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
	switch(token.type) {
		case TokenType::TOKEN_ILLEGAL: 
			assert(false);

		case TokenType::TOKEN_IDENT:
		case TokenType::LITERAL_INT:
		case TokenType::LITERAL_FLOAT:
		case TokenType::LITERAL_HEX:
		case TokenType::LITERAL_OCTAL:
		case TokenType::LITERAL_STRING:
			os << token.value;
			break;
			
		case TokenType::BOOL_TRUE:
		case TokenType::BOOL_FALSE:
		case TokenType::KEY_WORD_BREAK:
		case TokenType::KEY_WORD_CASE:
		case TokenType::KEY_WORD_CONST:
		case TokenType::KEY_WORD_CONTINUE:
		case TokenType::KEY_WORD_DEFAULT:
		case TokenType::KEY_WORD_DEFER:
		case TokenType::KEY_WORD_ELSE:
		case TokenType::KEY_WORD_FALLTHROUGH:
		case TokenType::KEY_WORD_FOR:
		case TokenType::KEY_WORD_FUNC:
		case TokenType::KEY_WORD_PROC:
		case TokenType::KEY_WORD_IF:
		case TokenType::KEY_WORD_IMPORT:
		case TokenType::KEY_WORD_RETURN:
		case TokenType::KEY_WORD_SELECT:
		case TokenType::KEY_WORD_STRUCT:
		case TokenType::KEY_WORD_SWITCH:
		case TokenType::KEY_WORD_TYPE:
		case TokenType::KEY_WORD_VAR:
		case TokenType::SYMBOL_SEMI:
		case TokenType::SYMBOL_COLON:
		case TokenType::SYMBOL_DEFINE:
		case TokenType::SYMBOL_DOUBLE_COLON:
		case TokenType::SYMBOL_ELLIPSE:
		case TokenType::SYMBOL_PERIOD:
		case TokenType::SYMBOL_COMMA:
		case TokenType::SYMBOL_LPAREN:
		case TokenType::SYMBOL_RPAREN:
		case TokenType::SYMBOL_LBRACK:
		case TokenType::SYMBOL_RBRACK:
		case TokenType::SYMBOL_LBRACE:
		case TokenType::SYMBOL_RBRACE:
		case TokenType::SYMBOL_ADD:
		case TokenType::SYMBOL_ADD_ASSIGN:
		case TokenType::SYMBOL_INC:
		case TokenType::SYMBOL_ARROW:
		case TokenType::SYMBOL_SUB:
		case TokenType::SYMBOL_SUB_ASSIGN:
		case TokenType::SYMBOL_DEC:
		case TokenType::SYMBOL_MUL:
		case TokenType::SYMBOL_MUL_ASSIGN:
		case TokenType::SYMBOL_QUO:
		case TokenType::SYMBOL_QUO_ASSIGN:
		case TokenType::SYMBOL_REM:
		case TokenType::SYMBOL_REM_ASSIGN:
		case TokenType::SYMBOL_XOR:
		case TokenType::SYMBOL_XOR_ASSIGN:
		case TokenType::SYMBOL_LSS:
		case TokenType::SYMBOL_LEQ:
		case TokenType::SYMBOL_SHL:
		case TokenType::SYMBOL_SHL_ASSIGN:
		case TokenType::SYMBOL_GTR:
		case TokenType::SYMBOL_GEQ:
		case TokenType::SYMBOL_SHR:
		case TokenType::SYMBOL_SHR_ASSIGN:
		case TokenType::SYMBOL_ASSIGN:
		case TokenType::SYMBOL_EQL:
		case TokenType::SYMBOL_NOT:
		case TokenType::SYMBOL_NEQ:
		case TokenType::SYMBOL_AND_NOT:
		case TokenType::SYMBOL_AND_NOT_ASSIGN:
		case TokenType::SYMBOL_AND:
		case TokenType::SYMBOL_AND_ASSIGN:
		case TokenType::SYMBOL_LAND:
		case TokenType::SYMBOL_OR:
		case TokenType::SYMBOL_OR_ASSIGN:
		case TokenType::SYMBOL_LOR:
			os << token.type;
			break;

		case TokenType::TOKEN_END: 
			os << ""; 
			break;
	}
	return os;
}