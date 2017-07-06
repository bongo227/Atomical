class Parser {
    private:
        std::deque<Token> tokens;
    
    public:
        explicit Parser(std::deque<Token>);
        explicit Parser(std::string source);
      
        std::vector<Function *> parse();
      
        Function *parse_function();
        Statement *parse_statement();
        Expression *parse_expression(int rbp);
        

    private:
        Token expect(TokenType type);
        void accept(TokenType type);
          
        ReturnStatement *parse_return_statement();
        BlockStatement *parse_block_statement();
        IfStatement *parse_if_statement();
        ForStatement *parse_for_statement();
        AssignStatement *parse_assign_statement();

        Expression *nud(Token token);
        Expression *led(Token token, Expression *expression);
        
        Type *parse_type();
};

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
    assert(tokens.front().type == type);
    Token token = tokens.front();
    tokens.pop_front();
    return token;
}

void Parser::accept(TokenType type) {
    if (tokens.front().type == type) tokens.pop_front();
}

ReturnStatement *Parser::parse_return_statement() {
    expect(TokenType::RETURN);
    ReturnStatement *smt = new ReturnStatement(parse_expression(0));;
    accept(TokenType::SEMI);
    return smt;
}

BlockStatement *Parser::parse_block_statement() {
    expect(TokenType::LBRACE);

    std::vector<Statement *> smts(0);
    while(tokens.front().type != TokenType::RBRACE) {
        smts.push_back(parse_statement());
    }

    expect(TokenType::RBRACE);
    
    return new BlockStatement(smts);
}

IfStatement *Parser::parse_if_statement() {
    expect(TokenType::IF);

    Expression *condition = parse_expression(0);
    BlockStatement *body = parse_block_statement();
    IfStatement *elses = NULL;

    if (tokens.front().type == TokenType::ELSE) {
        expect(TokenType::ELSE);
        if (tokens.front().type == TokenType::IF) {
            // else if so recursivly parse the chain.
            elses = parse_if_statement();
        } else {
            // final else statement
            elses = new IfStatement(NULL, NULL, parse_block_statement());
        }
    }

    return new IfStatement(condition, elses, body);
}

ForStatement *Parser::parse_for_statement() {
    expect(TokenType::FOR);

    Statement *declaration = parse_statement();
    Expression *condition = parse_expression(0);
    expect(TokenType::SEMI);
    Statement *increment = parse_statement();
    BlockStatement *body = parse_block_statement();

    return new ForStatement(declaration, condition, increment, body);
}

AssignStatement *Parser::parse_assign_statement() {
    IdentExpression *ident = new IdentExpression(expect(TokenType::IDENT).value);
    Token type = tokens.front();
    tokens.pop_front();
    Expression *value = parse_expression(0);
    return new AssignStatement(ident, type.type, value); 
}

Statement *Parser::parse_statement() {
    switch(tokens.front().type) {
        case TokenType::RETURN: return parse_return_statement();
        case TokenType::LBRACE: return parse_block_statement();
        case TokenType::IF: return parse_if_statement();
        case TokenType::FOR: return parse_for_statement();
        case TokenType::IDENT: return parse_assign_statement();
        default: assert(false); // expected statement
    }
    return NULL;
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

            return new CallExpression(static_cast<IdentExpression *>(expression), args);
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
        if (returns.size()) expect(TokenType::COMMA);
        Type *type = parse_type();
        Token ident = expect(TokenType::IDENT);
        std::string name = ident.value;
        returns.push_back(std::make_tuple(type, name));
    }

    BlockStatement *body = parse_block_statement();

    return new Function(ident.value, arguments, returns, body);
}