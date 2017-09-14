class Expression {
public:
    const bool is_bracketed_with_unary;

    Expression(bool is_bracketed_with_unary);
    
    virtual Value *code_gen(Irgen *irgen) const = 0;
    virtual bool is_equal(const Expression &exp) const = 0;
    virtual void print(std::ostream &os) const = 0; 

    friend bool operator==(const Expression& lhs, const Expression& rhs);
    friend bool operator!=(const Expression& lhs, const Expression& rhs);
    
    friend std::ostream& operator<<(std::ostream& os, const Expression& exp);
};

class IdentExpression : public Expression {
public:
    const std::string ident;

    IdentExpression(std::string ident);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const IdentExpression &exp);
};

class LiteralExpression : public Expression {
public:
    const TokenType type;
    const std::string value;

    // TODO: TokenType -> LiteralType
    LiteralExpression(TokenType type, std::string value);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const LiteralExpression &exp);
};

class UnaryExpression : public Expression {
public:
    const TokenType op_type;
    const Expression *exp;

    UnaryExpression(TokenType op_type, Expression *exp);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const UnaryExpression &exp);
};

class BinaryExpression : public Expression {
public:
    // BinaryTypw
    const TokenType op_type;
    const Expression *lhs;
    const Expression *rhs;

    BinaryExpression(TokenType op_type, Expression *lhs, Expression *rhs);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const BinaryExpression &exp);
};

class CallExpression : public Expression {
public:
    const std::string function_name;
    const std::vector<Expression *> args;

    CallExpression(std::string function_name, std::vector<Expression *> args);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const CallExpression &exp);
};

struct Statement {
    union {
        Expression *ret;
        std::vector<Statement *> block;
        struct { Expression *condition; Statement *body; Statement *elses; } ifs;
        struct { Expression *variable; TokenType type; Expression *value; } assign;
        struct { Statement *declaration; Expression *condition; Statement *increment; 
            Statement *body; } fors;
    };

    enum { RETURN, BLOCK, IF, ASSIGN, FOR } type;

    private:
        Statement(Expression *value);
        Statement(std::vector<Statement *> statements);
        Statement(Expression *condition, Statement *body, Statement *elses);
        Statement(Expression *variable, TokenType type, Expression *value);
        Statement(Statement *declaration, Expression *condition, Statement *increment, 
            Statement *body);

        bool is_equal(const Statement &smt) const;

    public:
        static Statement *Return(Expression *value)
            { return new Statement(value); }
        static Statement *Block(std::vector<Statement *> statements)
            { return new Statement(statements); }
        static Statement *If(Expression *condition, Statement *body, Statement *elses)
            { return new Statement(condition, body, elses); }
        static Statement *Assign(Expression *variable, TokenType type, Expression *value)
            { return new Statement(variable, type, value); }
        static Statement *For(Statement *declaration, Expression *condition, 
            Statement *increment, Statement *body) 
            { return new Statement(declaration, condition, increment, body); }

    friend bool operator==(const Statement& lhs, const Statement& rhs);
    friend bool operator!=(const Statement& lhs, const Statement& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Statement& smt);
};

struct Function {
    std::string name;
    std::vector<std::tuple<Type *, std::string>> arguments;
    std::vector<std::tuple<Type *, std::string>> returns;
    Statement *body;

    Function(std::string name, std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns, Statement *body);

    private:
        bool is_equal(const Function &func) const;

    friend std::ostream &operator<<(std::ostream& os, const Function& func);

    EQUAL_OP(Function);
    NOT_EQUAL_OP(Function);
};