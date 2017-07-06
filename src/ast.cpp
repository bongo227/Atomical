#include <typeinfo>
#include <iostream>
#include <vector>
#include <tuple>
#include <deque>

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
    private:
        virtual bool is_equal(const Expression& exp) const {
            return true;
        }

        virtual void print_node(std::ostream& os) const {
            os << "[BASE EXPRESSION]";
        }

    EQUAL_OP(Expression);
    NOT_EQUAL_OP(Expression);
    PRINT_OP(Expression);
};


struct IdentExpression : Expression {
    std::string ident;

    explicit IdentExpression(std::string ident) : ident(ident) {}
    
    private:
        virtual bool is_equal(const Expression& exp) const override {
            auto e = static_cast<const IdentExpression&>(exp);
            return this->ident == e.ident;
        }

        virtual void print_node(std::ostream& os) const override {
            os << ident;
        }

    PRINT_OP(IdentExpression)
};

struct LiteralExpression : Expression {
    TokenType type;
    std::string value;

    LiteralExpression(TokenType type, std::string value) : type(type), value(value) {}

    private:
        virtual bool is_equal(const Expression& exp) const override {
            auto e = static_cast<const LiteralExpression&>(exp);
            return this->type == e.type &&
                this->value == e.value;
        }

        virtual void print_node(std::ostream &os) const override {
            os << value;
        }
    
    PRINT_OP(LiteralExpression);
};

struct UnaryExpression : Expression {
    TokenType type;
    Expression *value;

    UnaryExpression(TokenType type, Expression *value) : type(type), value(value) {}

    private:
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

    private:
        virtual bool is_equal(const Expression& exp) const override {
            auto e = static_cast<const BinaryExpression&>(exp);
            return this->type == e.type &&
                *this->left == *e.left &&
                *this->right == *e.right;
        }

        virtual void print_node(std::ostream &os) const override {
            os << *left << " ";
            switch (type) {
                case TokenType::ADD: os << "+"; break;
                case TokenType::SUB: os << "-"; break;
                case TokenType::MUL: os << "*"; break;
                case TokenType::QUO: os << "/"; break;
                case TokenType::REM: os << "%"; break;
                case TokenType::EQL: os << "=="; break;
                case TokenType::NEQ: os << "!="; break;
                case TokenType::GTR: os << ">"; break;
                case TokenType::LSS: os << "<"; break;
                case TokenType::GEQ: os << ">="; break;
                case TokenType::LEQ: os << "<="; break;
                default: assert(false);
            }
            os << " " << *right;
        }

    PRINT_OP(BinaryExpression)
};

struct CallExpression : Expression {
    IdentExpression *function_name;
    std::vector<Expression *> args;

    CallExpression(IdentExpression *function_name, std::vector<Expression *> args) :
        function_name(function_name), args(args) {}

    private:
        virtual bool is_equal(const Expression& exp) const override {
            auto e = static_cast<const CallExpression&>(exp);
            if(*this->function_name != *e.function_name) return false;
            if (this->args.size() != e.args.size()) return false;
            for (size_t i = 0; i < e.args.size(); i++) {
                if(*this->args[i] != *e.args[i]) return false;
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
        // TODO: check pointers are not equal first (as done here) before dereference
        // in all other nodes.
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

struct ForStatement : Statement {
    Statement *declaration; // Change to assigment/define statement
    Expression *condition;
    Statement *increment;
    BlockStatement *body;

    ForStatement(Statement *declaration, Expression *condition, Statement *increment, 
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

struct AssignStatement : Statement {
    IdentExpression *variable;
    TokenType assign_type;
    Expression *value;

    AssignStatement(IdentExpression *variable, TokenType assign_type, Expression *value) 
        : variable(variable), assign_type(assign_type), value(value) {}

    private:
        virtual bool is_equal(const Statement &smt) const override {
            auto e = static_cast<const AssignStatement &>(smt);
            return *this->variable == *e.variable &&
                this->assign_type == e.assign_type &&
                *this->value == *e.value;
        }

        virtual void print_node(std::ostream &os) const override {
            os << *variable << " ";
            // TODO: add << operator to TokenType
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
                default: assert(false); // expected assignment operator
            }
            os << " " << *value;
        }

    PRINT_OP(AssignStatement)
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

struct PrimitiveType : Type {
    Primitive prim;

    explicit PrimitiveType(Primitive prim) : prim(prim) {}

    private:
        virtual bool is_equal(const Type& type) const override {
            auto t = static_cast<const PrimitiveType&>(type);
            return prim == t.prim;
        }

        virtual void print_node(std::ostream& os) const override {
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
            if(static_cast<Statement>(*this->body) != static_cast<Statement>(*func.body)) 
                return false;
            
            return true;
        }

    friend std::ostream& operator<<(std::ostream& os, const Function& func) {
        os << func.name << " :: ";
        for (size_t i = 0; i < func.arguments.size(); i++) {
            auto arg = func.arguments[i];
            // TODO: add type printing
            if(i > 0) os << ", ";
            os << "[TYPE] " << std::get<1>(arg);
        }
        if(func.arguments.size() != 0) os << " ";
        os << "-> ";
        for (size_t i = 0; i < func.returns.size(); i++) {
            auto ret = func.returns[i];
            // TODO: add type printing
            if(i > 0) os << ", ";
            os << "[TYPE] " << std::get<1>(ret);
        }
         if(func.returns.size() != 0) os << " ";
        os << *func.body;
        return os;
    }

    EQUAL_OP(Function);
    NOT_EQUAL_OP(Function);
};

