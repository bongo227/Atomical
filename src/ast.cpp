#define PRINT_OP(type)                                                              \
friend std::ostream& operator<<(std::ostream& os, const type& obj) {                \
    obj.print_node(os);                                                             \
    return os;                                                                      \
}

#define EQUAL_OP(type)                                                              \
friend bool operator==(const type& lhs, const type& rhs) {                          \
    return (typeid(lhs) == typeid(rhs)) && lhs.is_equal(rhs);                       \
}                                                                                   \

#define NOT_EQUAL_OP(type)                                                          \
friend bool operator!=(const type& lhs, const type& rhs) {                          \
    return (typeid(lhs) != typeid(rhs)) || !lhs.is_equal(rhs);                      \
}                                                                                   \

struct Expression {
    union {
        std::string ident;
        struct { TokenType type; std::string value; } literal;
        struct { TokenType type; Expression *value; } unary;
        struct { TokenType type; Expression *lhs; Expression *rhs; } binary;
        struct { std::string function_name; std::vector<Expression *> args; } call;
    };

    enum { IDENT, LITERAL, UNARY, BINARY, CALL } type;

    private:
        Expression(std::string ident) : ident(ident), type(IDENT) {}
        Expression(TokenType type, std::string value) : literal{type, value}, type(LITERAL) {}
        Expression(TokenType type, Expression *value) : unary{type, value}, type(UNARY) {}
        Expression(TokenType type, Expression *lhs, Expression *rhs)
            : binary{type, lhs, rhs}, type(BINARY) {}
        Expression(std::string function_name, std::vector<Expression *> args)
            : call{function_name, args}, type(CALL) {}
        
        bool is_equal(const Expression &exp) const {
            if (type != exp.type) return false;

            switch(type) {
                case IDENT:
                    return ident == exp.ident;
                case LITERAL:
                    return literal.type == exp.literal.type &&
                        literal.value == exp.literal.value;
                case UNARY:
                    return unary.type == exp.unary.type &&
                        *unary.value == *exp.unary.value;
                case BINARY:
                    return binary.type == exp.binary.type &&
                        *binary.lhs == *exp.binary.lhs &&
                        *binary.rhs == *exp.binary.rhs;
                case CALL:
                    if (call.function_name != exp.call.function_name) return false;
                    if (call.args.size() != exp.call.args.size()) return false;
                    for (int i = 0; i < call.args.size(); i++) {
                        if (*call.args[i] != *exp.call.args[i]) return false;
                    }
                    return true;
            }
            
            assert(false);
        }

    public:
        static Expression *Ident(std::string ident) 
            { return new Expression(ident); }
        static Expression *Literal(TokenType type, std::string value) 
            { return new Expression(type, value); }
        static Expression *Unary(TokenType type, Expression *value) 
            { return new Expression(type, value); }
        static Expression *Binary(TokenType type, Expression *lhs, Expression *rhs) 
            { return new Expression(type, lhs, rhs); }
        static Expression *Call(std::string function_name, std::vector<Expression *> args) 
            { return new Expression(function_name, args); }

    friend bool operator==(const Expression& lhs, const Expression& rhs) {
        return lhs.is_equal(rhs);
    }

    friend bool operator!=(const Expression& lhs, const Expression& rhs) {
        return !lhs.is_equal(rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const Expression& exp) {
        switch(exp.type) {
            case IDENT:
                os << exp.ident;
                break;
            case LITERAL:
                os << exp.literal.value;
                break;
            case UNARY:
                os << exp.unary.type;
                if (exp.unary.value->type == BINARY) os << "(";
                os << *exp.unary.value;
                if (exp.unary.value->type == BINARY) os << ")";
                break;
            case BINARY:
                os << *exp.binary.lhs << " " << exp.binary.type << " " << *exp.binary.rhs;
                break;
            case CALL:
                os << exp.call.function_name;
                os << "(";
                int i = 0;
                for (auto a : exp.call.args) {
                    if(i++ > 0) os << ", ";
                    os << *a;
                }
                os << ")";
                break;
        }
        return os;                                                         
    }
};

struct Statement {
    private:
        virtual bool is_equal(const Statement& exp) const {
            return true;
        }

        virtual void print_node(std::ostream& os) const {
            os << "[BASE STATEMENT]";
        } 

    EQUAL_OP(Statement);
    NOT_EQUAL_OP(Statement);
    PRINT_OP(Statement);
};


struct ReturnStatement : Statement {
    Expression *expression;

    explicit ReturnStatement(Expression *expression) : expression(expression) {}

    private:
        virtual bool is_equal(const Statement& smt) const override {
            auto e = static_cast<const ReturnStatement&>(smt);
            return *this->expression == *e.expression;
        }

        virtual void print_node(std::ostream& os) const override {
            os << "return " << *expression;
        }

    PRINT_OP(ReturnStatement)
};

struct BlockStatement : Statement {
    std::vector<Statement *> statements;

    explicit BlockStatement(std::vector<Statement *> statements) : statements(statements) {}

    private:
        virtual bool is_equal(const Statement& smt) const override {
            auto e = static_cast<const BlockStatement&>(smt);
            if (this->statements.size() != e.statements.size()) return false;
            for (size_t i = 0; i < e.statements.size(); i++) {
                if(!(*this->statements[i] == *e.statements[i])) return false;
            }
            return true;
        }

        virtual void print_node(std::ostream& os) const override {
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

    private:
        virtual bool is_equal(const Statement& smt) const override {
            auto e = static_cast<const IfStatement&>(smt);
            return (this->condition == e.condition || *this->condition == *e.condition) &&
                (this->elses == e.elses || *this->elses == *e.elses) &&
                (this->body == e.body || *this->body == *e.body);
        }

        virtual void print_node(std::ostream& os) const override {
            if (condition) os << "if " << *condition << " ";
            os << *body;
            if (elses) os << "else " << *elses; 
        }

    PRINT_OP(IfStatement)
};

struct AssignStatement : Statement {
    Expression *variable;
    TokenType assign_type;
    Expression *value;

    AssignStatement(Expression *variable, TokenType assign_type, Expression *value) 
        : variable(variable), assign_type(assign_type), value(value) {}

    private:
        virtual bool is_equal(const Statement &smt) const override {
            auto e = static_cast<const AssignStatement &>(smt);
            return *this->variable == *e.variable &&
                this->assign_type == e.assign_type &&
                *this->value == *e.value;
        }

        virtual void print_node(std::ostream &os) const override {
            os << *variable << " " << assign_type << " " << *value;
        }

    PRINT_OP(AssignStatement)
};

struct ForStatement : Statement {
    AssignStatement *declaration;
    Expression *condition;
    Statement *increment;
    BlockStatement *body;

    ForStatement(AssignStatement *declaration, Expression *condition, Statement *increment, 
        BlockStatement *body) : declaration(declaration), condition(condition), 
        increment(increment), body(body) {}

    private:
        virtual bool is_equal(const Statement& smt) const override {
            auto e = static_cast<const ForStatement&>(smt);
            return *this->declaration == *e.declaration &&
                *this->condition == *e.condition &&
                *this->increment == *e.increment &&
                *this->body == *e.body;
        }

        virtual void print_node(std::ostream& os) const override {
            os << "for " << *declaration << "; " << *condition << "; " 
                << *increment << " " << *body;
        }

    PRINT_OP(ForStatement)
};

struct Type {
    private:
        virtual bool is_equal(const Type &type) const {
            return true;
        }

        virtual void print_node(std::ostream& os) const {
            os << "[BASE TYPE]";
        }

    EQUAL_OP(Type);
    NOT_EQUAL_OP(Type);
    PRINT_OP(Type);
};

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

std::ostream &operator<<(std::ostream &os, const Primitive &prim) {
    switch(prim) {
        case Primitive::I8: os << "i8"; break;
        case Primitive::I16: os << "i16"; break;
        case Primitive::I32: os << "i32"; break;
        case Primitive::I64: os << "i64"; break;
        case Primitive::INT: os << "int"; break;
        case Primitive::F32: os << "f32"; break;
        case Primitive::F64: os << "f64"; break;
        case Primitive::FLOAT: os << "float"; break;
    }
    return os;
}

struct PrimitiveType : Type {
    Primitive prim;

    explicit PrimitiveType(Primitive prim) : prim(prim) {}

    private:
        virtual bool is_equal(const Type& type) const override {
            auto t = static_cast<const PrimitiveType&>(type);
            return prim == t.prim;
        }

        virtual void print_node(std::ostream& os) const override {
            os << prim;
        }
};

struct Function {
    std::string name;
    std::vector<std::tuple<Type *, std::string>> arguments;
    std::vector<std::tuple<Type *, std::string>> returns;
    BlockStatement *body;

    Function(std::string name,
        std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns,
        BlockStatement *body)
        : name(name), arguments(arguments), returns(returns), body(body) {}

    private:
        bool is_equal(const Function &func) const {
            if(this->name != func.name) return false;
            
            // Check arguments are equal
            if(this->arguments.size() != func.arguments.size()) return false;
            for (size_t i = 0; i < this->arguments.size(); i++) {
                auto type = std::get<0>(this->arguments[i]);
                auto func_type = std::get<0>(func.arguments[i]);
                if(*type != *func_type) return false;

                auto name = std::get<1>(this->arguments[i]);
                auto func_name = std::get<1>(func.arguments[i]);
                if(name != func_name) return false;
            }
            
            // Check returns are equal
            if(this->returns.size() != func.returns.size()) return false;
            for (size_t i = 0; i < this->returns.size(); i++) {
                auto type = std::get<0>(this->returns[i]);
                auto func_type = std::get<0>(func.returns[i]);
                if(*type != *func_type) return false;

                auto name = std::get<1>(this->returns[i]);
                auto func_name = std::get<1>(func.returns[i]);
                if(name != func_name) return false;
            }

            // Check bodys are equal
            if(*static_cast<Statement *>(this->body) != *static_cast<Statement *>(func.body)) 
                return false;
            
            return true;
        }

    friend std::ostream& operator<<(std::ostream& os, const Function& func) {
        os << "proc " << func.name << " :: ";
        for (size_t i = 0; i < func.arguments.size(); i++) {
            auto arg = func.arguments[i];
            if(i > 0) os << ", ";
            os << *std::get<0>(arg) << " " << std::get<1>(arg);
        }
        if(func.arguments.size() != 0) os << " ";
        os << "-> ";
        for (size_t i = 0; i < func.returns.size(); i++) {
            auto ret = func.returns[i];
            if(i > 0) os << ", ";
            os << *std::get<0>(ret) << " " << std::get<1>(ret);
        }
         if(func.returns.size() != 0) os << " ";
        os << *func.body;
        return os;
    }

    EQUAL_OP(Function);
    NOT_EQUAL_OP(Function);
};
