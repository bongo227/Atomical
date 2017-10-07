Parser::Parser(std::deque<Token> tokens) : tokens(tokens) {}

Parser::Parser(std::string source) : Parser(Lexer(source).lex()) {}

std::vector<Function *> Parser::parse() {
    std::vector<Function *> functions(0);
    while (tokens.size()) {
        functions.push_back(parse_function());
    }

    return functions;
}

Token Parser::expect(enum TokenType type) {
    if (tokens.front().type != type) {
        std::cout << "Expected: \"" << type << "\"" << std::endl;
        std::cout << "Got: \"" << tokens.front().type << "\"" << std::endl;
        assert(false);
    }
    Token token = tokens.front();
    tokens.pop_front();
    return token;
}

void Parser::accept(enum TokenType type) {
    if (tokens.front().type == type) tokens.pop_front();
}

ReturnStatement *Parser::parse_return_statement() {
    expect(TokenType::KEY_WORD_RETURN);
    return new ReturnStatement(parse_expression(0));;
}

BlockStatement *Parser::parse_block_statement() {
    expect(TokenType::SYMBOL_LBRACE);

    std::vector<Statement *> smts(0);
    while(tokens.front().type != TokenType::SYMBOL_RBRACE) {
        smts.push_back(parse_statement());
    }

    expect(TokenType::SYMBOL_RBRACE);
    
    return new BlockStatement(smts);
}

IfStatement *Parser::parse_if_statement() {
    expect(TokenType::KEY_WORD_IF);

    Expression *condition = parse_expression(0);
    BlockStatement *body = parse_block_statement();
    IfStatement *elses = NULL;

    if (tokens.front().type == TokenType::KEY_WORD_ELSE) {
        expect(TokenType::KEY_WORD_ELSE);
        if (tokens.front().type == TokenType::KEY_WORD_IF) {
            // else if so recursivly parse the chain.
            elses = parse_if_statement();
        } else {
            // final else statement
            elses = new IfStatement(NULL, parse_block_statement(), NULL);
        }
    }

    return new IfStatement(condition, body, elses);
}

ForStatement *Parser::parse_for_statement() {
    expect(TokenType::KEY_WORD_FOR);

    Statement *declaration = parse_assign_statement();
    expect(TokenType::SYMBOL_SEMI);
    Expression *condition = parse_expression(0);
    expect(TokenType::SYMBOL_SEMI);
    Statement *increment = parse_assign_statement();
    Statement *body = parse_block_statement();

    return new ForStatement(declaration, condition, increment, body);
}

AssignStatement *Parser::parse_assign_statement() {
    Expression *ident = new IdentExpression(expect(TokenType::TOKEN_IDENT).value);
    Token type = tokens.front();
    tokens.pop_front();
    Expression *value = parse_expression(0);
    return new AssignStatement(ident, type.type, value); 
}

Statement *Parser::parse_statement() {
    Statement *statement = nullptr;
    switch(tokens.front().type) {
        case TokenType::KEY_WORD_RETURN: {
            statement = parse_return_statement();
            break;
        } 
        case TokenType::SYMBOL_LBRACE: {
            statement = parse_block_statement();
            break;
        } 
        case TokenType::KEY_WORD_IF: {
            statement = parse_if_statement();
            break;
        } 
        case TokenType::TOKEN_IDENT: {
            statement = parse_assign_statement();
            break;
        } 
        case TokenType::KEY_WORD_FOR: {
            statement = parse_for_statement();
            break;
        } 
        default: {
            std::cout << "Expected statement, got token: \"" 
                      << tokens.front().type
                      << "\""
                      << std::endl;
            assert(false);
            break;
        }
    }
    expect(TokenType::SYMBOL_SEMI);
    return statement;
}

Expression *Parser::parse_expression(int rbp) {
    Expression *left;
    Token t = tokens.front();
    tokens.pop_front();
    left = nud(t);

    while (rbp < tokens.front().get_binding_power()) {
        t = tokens.front();
        tokens.pop_front();
        left = led(t, left);
    }

    return left;
}

Expression *Parser::nud(Token token) {
    switch(token.type) {
        case TokenType::TOKEN_IDENT: {
            return new IdentExpression(token.value);
        }

        case TokenType::BOOL_FALSE: {
            return new LiteralExpression(token.type, "false");
        }

        case TokenType::BOOL_TRUE: {
            return new LiteralExpression(token.type, "true");
        }

        case TokenType::LITERAL_INT:
        case TokenType::LITERAL_FLOAT:
        case TokenType::LITERAL_HEX:
        case TokenType::LITERAL_OCTAL:
        case TokenType::LITERAL_STRING: {
            return new LiteralExpression(token.type, token.value);
        }

        case TokenType::SYMBOL_NOT:
        case TokenType::SYMBOL_SUB: {
            return new UnaryExpression(token.type, parse_expression(60));
        }

        default:
            std::cout << "Expected prefix token, got token: \"" 
                << token.type
                << "\""
                << std::endl;
            assert(false); // Expected a prefix token
    }
    return NULL;
}

Expression *Parser::led(Token token, Expression *expression) {
    int bp = token.get_binding_power();
    switch(token.type) {
        case TokenType::SYMBOL_ADD:
        case TokenType::SYMBOL_SUB:
        case TokenType::SYMBOL_MUL:
        case TokenType::SYMBOL_QUO:
        case TokenType::SYMBOL_REM:
        case TokenType::SYMBOL_EQL:
        case TokenType::SYMBOL_NEQ:
        case TokenType::SYMBOL_GTR:
        case TokenType::SYMBOL_LSS:
        case TokenType::SYMBOL_GEQ:
        case TokenType::SYMBOL_LEQ: {
            return new BinaryExpression(token.type, expression, parse_expression(bp));
        }
        
        case TokenType::SYMBOL_LPAREN: {
            std::vector<Expression *> args(0);
            while(tokens.front().type != TokenType::SYMBOL_RPAREN) {
                if(args.size()) expect(TokenType::SYMBOL_COMMA);
                args.push_back(parse_expression(0)); // crash
            }
            expect(TokenType::SYMBOL_RPAREN);

            // TODO: find a way to remove this cast
            return new CallExpression(static_cast<IdentExpression *>(expression)->ident, args);
        }

        default:
            assert(false); // Expected infix expression
    }
    return NULL;
}

Type *Parser::parse_type() {
    Token ident = expect(TokenType::TOKEN_IDENT);
    if(ident.value == "i8") return new PrimitiveType(Primitive::I8);
    else if(ident.value == "i16") return new PrimitiveType(Primitive::I16);
    else if(ident.value == "i32") return new PrimitiveType(Primitive::I32);
    else if(ident.value == "i64") return new PrimitiveType(Primitive::I64);
    else if(ident.value == "int") return new PrimitiveType(Primitive::INT);
    else if(ident.value == "f32") return new PrimitiveType(Primitive::F32);
    else if(ident.value == "f64") return new PrimitiveType(Primitive::F64);
    else if(ident.value == "float") return new PrimitiveType(Primitive::FLOAT);
    else assert(false); // Type is not primitive

    return NULL;
}

Function *Parser::parse_function() {
    expect(TokenType::KEY_WORD_PROC);

    Token ident = expect(TokenType::TOKEN_IDENT);

    expect(TokenType::SYMBOL_DOUBLE_COLON);

    std::vector<std::tuple<Type *, std::string>> arguments;
    while(tokens.front().type != TokenType::SYMBOL_ARROW) {
        if (arguments.size()) expect(TokenType::SYMBOL_COMMA);
        Type *type = parse_type();
        Token ident = expect(TokenType::TOKEN_IDENT);
        std::string name = ident.value;
        arguments.push_back(std::make_tuple(type, name));
    }

    expect(TokenType::SYMBOL_ARROW);

    std::vector<std::tuple<Type *, std::string>> returns;
    while(tokens.front().type != TokenType::SYMBOL_LBRACE) {
        if (returns.size()) 
            expect(TokenType::SYMBOL_COMMA);
        
        Type *type = parse_type();
        
        std::string name = "";
        if(tokens.front().type == TokenType::TOKEN_IDENT)
            name = expect(TokenType::TOKEN_IDENT).value;

        returns.push_back(std::make_tuple(type, name));
    }

    BlockStatement *body = parse_block_statement();

    expect(TokenType::SYMBOL_SEMI);
    
    return new Function(ident.value, arguments, returns, body);
}