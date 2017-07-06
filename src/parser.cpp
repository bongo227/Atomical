#include <typeinfo>
#include <iostream>

#define PRINT_OP(type)                                                              \
friend std::ostream& operator<<(std::ostream& os, const type& obj) {                \
    obj.print_node(os);                                                             \
    return os;                                                                      \
}                                                                                   \

struct Function;
struct Statement;
struct Expression;
struct Type;
struct BlockStatement;
class Parser;

struct Node {
    public:
        Node *left;
        Node *right;
};

struct Expression : Node {
    virtual bool is_equal(const Expression& exp) const {
       return true;
    }

    virtual void print_node(std::ostream& os) const {
        os << "[BASE EXPRESSION]";
    } 

    PRINT_OP(Expression);
};

bool operator==(const Expression& lhs, const Expression& rhs) {
    return (typeid(lhs) == typeid(rhs)) && lhs.is_equal(rhs);
}

struct IdentExpression : Expression {
    std::string ident;

    explicit IdentExpression(std::string ident) : ident(ident) {}
    
    virtual bool is_equal(const Expression& exp) const override {
        auto e = static_cast<const IdentExpression&>(exp);
        return this->ident == e.ident;
    }

    virtual void print_node(std::ostream& os) const {
        os << ident;
    }

    PRINT_OP(IdentExpression)
};

struct LiteralExpression : Expression {
    TokenType type;
    std::string value;

    LiteralExpression(TokenType type, std::string value) : type(type), value(value) {}

    virtual bool is_equal(const Expression& exp) const override {
        auto e = static_cast<const LiteralExpression&>(exp);
        return this->type == e.type &&
            this->value == e.value;
    }
};

struct UnaryExpression : Expression {
    TokenType type;
    Expression *value;

    UnaryExpression(TokenType type, Expression *value) : type(type), value(value) {}

    virtual bool is_equal(const Expression& exp) const override {
        auto e = static_cast<const UnaryExpression&>(exp);
        return this->type == e.type &&
            *this->value == *e.value;
    }

    virtual void print_node(std::ostream& os) const override {
        if(type == TokenType::SUB) os << "-";
        else if(type == TokenType::NOT) os << "!";
        else assert(false);
        os << *value;
    }

    PRINT_OP(UnaryExpression)
};

struct BinaryExpression : Expression {
    TokenType type;
    Expression *left;
    Expression *right;

    BinaryExpression(TokenType type, Expression *left, Expression *right)
     : type(type), left(left), right(right) {}

    virtual bool is_equal(const Expression& exp) const override {
        auto e = static_cast<const BinaryExpression&>(exp);
        return this->type == e.type &&
            this->left && e.left &&
            this->right && e.right;
    }
};

struct CallExpression : Expression {
    IdentExpression *function_name;
    std::vector<Expression *> args;

    CallExpression(IdentExpression *function_name, std::vector<Expression *> args) :
        function_name(function_name), args(args) {}

    virtual bool is_equal(const Expression& exp) const override {
        auto e = static_cast<const CallExpression&>(exp);
        if (this->args.size() != e.args.size()) return false;
        for (int i = 0; i < e.args.size(); i++) {
            if(!(*this->args[i] == *e.args[i])) return false;
        }
        return true;
    }

    virtual void print_node(std::ostream& os) const override {
        os << *function_name;
        os << "(";
        int i = 0;
        for (auto a : args) {
            if(i++ > 0) os << ", ";
            os << *a;
        }
        os << ")";
    }

    PRINT_OP(CallExpression)
};

struct Statement : Node {
    virtual bool is_equal(const Statement& exp) const {
       return true;
    }

    virtual void print_node(std::ostream& os) const {
        os << "[BASE STATEMENT]";
    } 

    PRINT_OP(Statement);
};

bool operator==(const Statement& lhs, const Statement& rhs) {
    return (typeid(lhs) == typeid(rhs)) && lhs.is_equal(rhs);
}

struct ReturnStatement : Statement {
    Expression *expression;

    explicit ReturnStatement(Expression *expression) : expression(expression) {}

    virtual bool is_equal(const Statement& smt) const override {
        auto e = static_cast<const ReturnStatement&>(smt);
        return *this->expression == *e.expression;
    }

    virtual void print_node(std::ostream& os) const {
        os << "return " << *expression;
    }

    PRINT_OP(ReturnStatement)
};

struct BlockStatement : Statement {
    std::vector<Statement *> statements;

    explicit BlockStatement(std::vector<Statement *> statements) : statements(statements) {}

    virtual bool is_equal(const Statement& smt) const override {
        auto e = static_cast<const BlockStatement&>(smt);
        if (this->statements.size() != e.statements.size()) return false;
        for (int i = 0; i < e.statements.size(); i++) {
            if(!(*this->statements[i] == *e.statements[i])) return false;
        }
        return true;
    }

    virtual void print_node(std::ostream& os) const {
        os << "{" << std::endl;
        for (auto s : statements) {
            os << "  " << *s << std::endl;
        }
        os << "}" << std::endl;
    }

    PRINT_OP(BlockStatement)
};

struct IfStatement : Statement {
    Expression *condition;
    IfStatement *elses;
    BlockStatement *body;

    IfStatement(Expression *condition, IfStatement *elses, BlockStatement *body) : 
        condition(condition), elses(elses), body(body) {}

    // TODO: check pointers are not equal first (as done here) before dereference
    // in all other nodes.
    virtual bool is_equal(const Statement& smt) const override {
        auto e = static_cast<const IfStatement&>(smt);
        return (this->condition == e.condition || *this->condition == *e.condition) &&
            (this->elses == e.elses || *this->elses == *e.elses) &&
            (this->body == e.body || *this->body == *e.body);
    }

    virtual void print_node(std::ostream& os) const {
        os << "if " << *condition << " " << *body;
        if (this->elses != NULL) os << "else " << *elses; 
    }

    PRINT_OP(IfStatement)
};

struct ForStatement : Statement {
    Statement *declaration;
    Expression *condition;
    Statement *increment;
    BlockStatement *body;

    ForStatement(Statement *declaration, Expression *condition, Statement *increment, 
        BlockStatement *body) : declaration(declaration), condition(condition), 
        increment(increment), body(body) {}

    virtual bool is_equal(const Statement& smt) const override {
        auto e = static_cast<const ForStatement&>(smt);
        return *this->declaration == *e.declaration &&
            *this->condition == *e.condition &&
            *this->increment == *e.increment &&
            *this->body == *e.body;
    }

    virtual void print_node(std::ostream& os) const {
        os << "for " << *declaration << "; " << *condition << "; " << *increment << *body;
    }

    PRINT_OP(ForStatement)
};

struct AssignStatement : Statement {
    IdentExpression *variable;
    TokenType assign_type;
    Expression *value;

    AssignStatement(IdentExpression *variable, TokenType assign_type, Expression *value) 
        : variable(variable), assign_type(assign_type), value(value) {}

    virtual bool is_equal(const Statement &smt) const override {
        auto e = static_cast<const AssignStatement &>(smt);
        return *this->variable == *e.variable &&
            this->assign_type == e.assign_type &&
            *this->value == *e.value;
    }

    virtual void print_node(std::ostream &os) const {
        os << *variable << " ";
        switch (assign_type) {
            case TokenType::DEFINE: os << ":="; break;
            case TokenType::ASSIGN: os << "="; break;
            case TokenType::ADD_ASSIGN: os << "+="; break;
            case TokenType::SUB_ASSIGN: os << "-="; break;
            case TokenType::MUL_ASSIGN: os << "*="; break;
            case TokenType::QUO_ASSIGN: os << "/="; break;
            case TokenType::REM_ASSIGN: os << "%="; break;
            case TokenType::XOR_ASSIGN: os << "^="; break;
            case TokenType::SHL_ASSIGN: os << "<<="; break;
            case TokenType::SHR_ASSIGN: os << ">>="; break;
            case TokenType::AND_NOT_ASSIGN: os << "&^="; break;
            case TokenType::AND_ASSIGN: os << "&="; break;
            case TokenType::OR_ASSIGN: os << "|="; break;
        }
        os << *value;
    }

    PRINT_OP(AssignStatement)
};

struct Type : Node {};

enum class Primitive {
    I8,
    I16,
    I32,
    I64,
    INT,
    F32,
    F64,
    FLOAT,
};

struct PrimitiveType : Type {
    Primitive type;

    explicit PrimitiveType(Primitive type) : type(type) {}
};


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
    auto ident = parse_expression(0); // TODO: change to parse_ident_expression()
    auto type = tokens.front();
    tokens.pop_front();
    auto *value = parse_expression(0);
    return new AssignStatement(static_cast<IdentExpression *>(ident), type.type, value); 
}

Statement *Parser::parse_statement() {
    switch(tokens.front().type) {
        case TokenType::RETURN: return parse_return_statement();
        case TokenType::LBRACE: return parse_block_statement();
        case TokenType::IF: return parse_if_statement();
        case TokenType::FOR: return parse_for_statement();
        case TokenType::IDENT: return parse_assign_statement();
    }

    assert(false); // expected statement
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

            return new CallExpression(new IdentExpression(token.value), args);
        }

        default:
            assert(false); // Expected infix expression
    }
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

struct Function : Node {
    std::string name;
    std::vector<std::tuple<Type *, std::string>> arguments;
    std::vector<std::tuple<Type *, std::string>> returns;
    BlockStatement *body;

    Function(std::string name,
        std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns,
        BlockStatement *body)
        : name(name), arguments(arguments), returns(returns), body(body) {}

    friend std::ostream& operator<<(std::ostream& os, const Function& func) {
        os << func.name << " :: ";
        for (int i = 0; i < func.arguments.size(); i++) {
            auto arg = func.arguments[i];
            // TODO: add type printing
            if(i > 0) os << ", ";
            os << "[TYPE] " << std::get<1>(arg);
        }
        os << " -> ";
        for (int i = 0; i < func.returns.size(); i++) {
            auto ret = func.returns[i];
            // TODO: add type printing
            if(i > 0) os << ", ";
            os << "[TYPE] " << std::get<1>(ret);
        }
        os << *func.body;
        return os;
    }
};

bool operator==(const Function& lhs, const Function& rhs) {
    if(lhs.name != rhs.name) return false;
    if(lhs.arguments.size() != rhs.arguments.size()) return false;
    if(lhs.returns.size() != rhs.returns.size()) return false;
    // TODO: Check arguments and returns are actually the same, not just the same length
    if(!(static_cast<Statement>(*lhs.body) == static_cast<Statement>(*rhs.body))) return false;
    return true;
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