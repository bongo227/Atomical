Parser::Parser(std::deque<Token> tokens) : tokens(tokens) {}

Parser::Parser(std::string source) : Parser(Lexer(source).lex()) {}

std::vector<Function *> Parser::parse() {
    std::vector<Function *> functions(0);
    while (tokens.size()) {
        functions.push_back(parse_function());
    }

    return functions;
}

Token Parser::expect(TokenType type) {
    if (tokens.front().type != type) {
        std::cout << "Expected: \"" << type << "\"" << std::endl;
        std::cout << "Got: \"" << tokens.front().type << "\"" << std::endl;
        assert(false);
    }
    Token token = tokens.front();
    tokens.pop_front();
    return token;
}

void Parser::accept(TokenType type) {
    if (tokens.front().type == type) tokens.pop_front();
}

Statement *Parser::parse_return_statement() {
    expect(TokenType::RETURN);
    Statement *smt = Statement::Return(parse_expression(0));;
    return smt;
}

Statement *Parser::parse_block_statement() {
    expect(TokenType::LBRACE);

    std::vector<Statement *> smts(0);
    while(tokens.front().type != TokenType::RBRACE) {
        smts.push_back(parse_statement());
    }

    expect(TokenType::RBRACE);
    
    return Statement::Block(smts);
}

Statement *Parser::parse_if_statement() {
    expect(TokenType::IF);

    Expression *condition = parse_expression(0);
    Statement *body = parse_block_statement();
    Statement *elses = NULL;

    if (tokens.front().type == TokenType::ELSE) {
        expect(TokenType::ELSE);
        if (tokens.front().type == TokenType::IF) {
            // else if so recursivly parse the chain.
            elses = parse_if_statement();
        } else {
            // final else statement
            elses = Statement::If(NULL, parse_block_statement(), NULL);
        }
    }

    return Statement::If(condition, body, elses);
}

Statement *Parser::parse_for_statement() {
    expect(TokenType::FOR);

    Statement *declaration = parse_assign_statement();
    expect(TokenType::SEMI);
    Expression *condition = parse_expression(0);
    expect(TokenType::SEMI);
    Statement *increment = parse_assign_statement();
    Statement *body = parse_block_statement();

    return Statement::For(declaration, condition, increment, body);
}

Statement *Parser::parse_assign_statement() {
    Expression *ident = new IdentExpression(expect(TokenType::IDENT).value);
    Token type = tokens.front();
    tokens.pop_front();
    Expression *value = parse_expression(0);
    return Statement::Assign(ident, type.type, value); 
}

Statement *Parser::parse_statement() {
    Statement *statement = nullptr;
    switch(tokens.front().type) {
        case TokenType::RETURN: {
            statement = parse_return_statement();
            break;
        } 
        case TokenType::LBRACE: {
            statement = parse_block_statement();
            break;
        } 
        case TokenType::IF: {
            statement = parse_if_statement();
            break;
        } 
        case TokenType::IDENT: {
            statement = parse_assign_statement();
            break;
        } 
        case TokenType::FOR: {
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
    expect(TokenType:: SEMI);
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
        case TokenType::IDENT: {
            return new IdentExpression(token.value);
        }

        case TokenType::BOOL_FALSE: {
            return new LiteralExpression(token.type, "false");
        }

        case TokenType::BOOL_TRUE: {
            return new LiteralExpression(token.type, "true");
        }

        case TokenType::INT:
        case TokenType::FLOAT:
        case TokenType::HEX:
        case TokenType::OCTAL:
        case TokenType::STRING: {
            return new LiteralExpression(token.type, token.value);
        }

        case TokenType::NOT: 
        case TokenType::SUB: {
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
        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::MUL:
        case TokenType::QUO:
        case TokenType::REM:
        case TokenType::EQL:
        case TokenType::NEQ:
        case TokenType::GTR:
        case TokenType::LSS:
        case TokenType::GEQ:
        case TokenType::LEQ: {
            return new BinaryExpression(token.type, expression, parse_expression(bp));
        }
        
        case TokenType::LPAREN: {
            std::vector<Expression *> args(0);
            while(tokens.front().type != TokenType::RPAREN) {
                if(args.size()) expect(TokenType::COMMA);
                args.push_back(parse_expression(0)); // crash
            }
            expect(TokenType::RPAREN);

            // TODO: find a way to remove this cast
            return new CallExpression(static_cast<IdentExpression *>(expression)->ident, args);
        }

        default:
            assert(false); // Expected infix expression
    }
    return NULL;
}

Type *Parser::parse_type() {
    Token ident = expect(TokenType::IDENT);
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
    expect(TokenType::PROC);

    Token ident = expect(TokenType::IDENT);

    expect(TokenType::DOUBLE_COLON);

    std::vector<std::tuple<Type *, std::string>> arguments;
    while(tokens.front().type != TokenType::ARROW) {
        if (arguments.size()) expect(TokenType::COMMA);
        Type *type = parse_type();
        Token ident = expect(TokenType::IDENT);
        std::string name = ident.value;
        arguments.push_back(std::make_tuple(type, name));
    }

    expect(TokenType::ARROW);

    std::vector<std::tuple<Type *, std::string>> returns;
    while(tokens.front().type != TokenType::LBRACE) {
        if (returns.size()) 
            expect(TokenType::COMMA);
        
        Type *type = parse_type();
        
        std::string name = "";
        if(tokens.front().type == TokenType::IDENT)
            name = expect(TokenType::IDENT).value;

        returns.push_back(std::make_tuple(type, name));
    }

    Statement *body = parse_block_statement();

    expect(TokenType::SEMI);
    
    return new Function(ident.value, arguments, returns, body);
}