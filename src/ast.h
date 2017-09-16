class Expression {
public:
    const bool is_bracketed_with_unary; // does have brackets when wrapped in unary operator 

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
};

class UnaryExpression : public Expression {
public:
    const TokenType op_type;
    const Expression *exp;

    UnaryExpression(TokenType op_type, Expression *exp);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
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
};

class CallExpression : public Expression {
public:
    const std::string function_name;
    const std::vector<Expression *> args;

    CallExpression(std::string function_name, std::vector<Expression *> args);

    Value *code_gen(Irgen *irgen) const;
    bool is_equal(const Expression &exp) const;
    void print(std::ostream &os) const;
};

struct Statement {
public:
    Statement();

    virtual void code_gen(Irgen *irgen) const = 0;
    virtual bool is_equal(const Statement &smt) const = 0;
    virtual void print(std::ostream &os) const = 0; 
    
    friend bool operator==(const Statement& lhs, const Statement& rhs);
    friend bool operator!=(const Statement& lhs, const Statement& rhs);    
    friend std::ostream& operator<<(std::ostream& os, const Statement& smt);
};

class ReturnStatement : public Statement {
public:
    const Expression *exp;

    ReturnStatement(Expression *exp);

    void code_gen(Irgen *irgen) const;
    bool is_equal(const Statement &smt) const;
    void print(std::ostream &os) const;
};

class BlockStatement : public Statement {
public:
    const std::vector<Statement *> statements;

    BlockStatement(std::vector<Statement *> statements);

    void code_gen(Irgen *irgen) const;
    bool is_equal(const Statement &smt) const;
    void print(std::ostream &os) const;
};

class IfStatement : public Statement {
public:
    const Expression *condition; 
    const BlockStatement *body; 
    const IfStatement *elses;

    IfStatement(Expression *condition, BlockStatement *body, IfStatement *elses);

    void code_gen(Irgen *irgen) const;
    void code_gen_if_branch(Irgen *irgen, const IfStatement *ifs, BasicBlock *parent_block, 
        BasicBlock *end_block) const;
    bool is_equal(const Statement &smt) const;
    void print(std::ostream &os) const;
};

class AssignStatement : public Statement {
public:
    const Expression *variable; 
    const TokenType op_type; 
    const Expression *value;

    AssignStatement(Expression *variable, TokenType op_type, Expression *value);

    void code_gen(Irgen *irgen) const;
    bool is_equal(const Statement &smt) const;
    void print(std::ostream &os) const;
};

class ForStatement : public Statement {
public:
    const Statement *declaration; 
    const Expression *condition; 
    const Statement *increment; 
    const Statement *body;

    ForStatement(Statement *declaration, Expression *condition, Statement *increment, 
        Statement *body);

    void code_gen(Irgen *irgen) const;
    bool is_equal(const Statement &smt) const;
    void print(std::ostream &os) const;
};

struct Function {
public:
    const std::string name;
    const std::vector<std::tuple<Type *, std::string>> arguments;
    const std::vector<std::tuple<Type *, std::string>> returns;
    const BlockStatement *body;

    Function(std::string name, std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns, BlockStatement *body);

    private:
        bool is_equal(const Function &func) const;

    friend std::ostream &operator<<(std::ostream& os, const Function& func);

    EQUAL_OP(Function);
    NOT_EQUAL_OP(Function);
};