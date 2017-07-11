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
        Statement(Expression *value) : ret(value), type(RETURN) {}
        Statement(std::vector<Statement *> statements) : block(statements), type(BLOCK) {}
        Statement(Expression *condition, Statement *body, Statement *elses) 
            : ifs{condition, body, elses}, type(IF) { assert(body); }
        Statement(Expression *variable, TokenType type, Expression *value)
            : assign{variable, type, value}, type(ASSIGN) {}
        Statement(Statement *declaration, Expression *condition, Statement *increment, 
            Statement *body) : fors{declaration, condition, increment, body}, type(FOR) {}

        bool is_equal(const Statement &smt) const {
            if (type != smt.type) return false;

            switch(type) {
                case RETURN:
                    return *ret == *smt.ret;
                
                case BLOCK:
                    if (block.size() != smt.block.size()) return false;
                    for (size_t i = 0; i < smt.block.size(); i++) {
                        if(!(*block[i] == *smt.block[i])) return false;
                    }
                    return true;
                
                case IF:
                    return (ifs.condition == smt.ifs.condition || 
                        *ifs.condition == *smt.ifs.condition) &&
                        (ifs.elses == smt.ifs.elses || *ifs.elses == *smt.ifs.elses) &&
                        (ifs.body == smt.ifs.body || *ifs.body == *smt.ifs.body);

                case ASSIGN:
                    return *assign.variable == *smt.assign.variable &&
                        assign.type == smt.assign.type &&
                        *assign.value == *smt.assign.value;

                case FOR:
                    return *fors.declaration == *smt.fors.declaration &&
                        *fors.condition == *smt.fors.condition &&
                        *fors.increment == *smt.fors.increment &&
                        *fors.body == *smt.fors.body;
            }
            
            assert(false);
        }

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

    friend bool operator==(const Statement& lhs, const Statement& rhs) {
        return lhs.is_equal(rhs);
    }

    friend bool operator!=(const Statement& lhs, const Statement& rhs) {
        return !lhs.is_equal(rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const Statement& smt) {
        switch(smt.type) {
            case RETURN:
                os << "return " << *smt.ret;
                break;

            case BLOCK:
                os << "{" << std::endl;
                for (auto s : smt.block) {
                    os << "  " << *s << std::endl;
                }
                os << "}" << std::endl;       
                break;

            case IF:
                if (smt.ifs.condition) os << "if " << *smt.ifs.condition << " ";
                os << *smt.ifs.body;
                if (smt.ifs.elses) os << "else " << *smt.ifs.elses;
                break;

            case ASSIGN:
                os << *smt.assign.variable << " " << smt.assign.type << " " 
                    << *smt.assign.value;
                break;

            case FOR:
                os << "for " << *smt.fors.declaration << "; " << *smt.fors.condition << "; " 
                    << *smt.fors.increment << " " << *smt.fors.body;
                break;
        }
        
        return os;
    }
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
    explicit PrimitiveType(TokenType type) {
        switch(type) {
            case TokenType::INT:
            case TokenType::HEX:
            case TokenType::OCTAL: 
                PrimitiveType(Primitive::INT); 
                break;
            
            case TokenType::FLOAT: 
                PrimitiveType(Primitive::FLOAT); 
                break;
            
            default:
                assert(false); // not a primitive type
        }
    }

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
    Statement *body;

    Function(std::string name,
        std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns,
        Statement *body)
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
