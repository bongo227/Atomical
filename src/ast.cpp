Expression::Expression(bool is_bracketed_with_unary) :
    is_bracketed_with_unary(is_bracketed_with_unary) {}

bool operator==(const Expression& lhs, const Expression& rhs) {
    return typeid(lhs) == typeid(rhs) && lhs.is_equal(rhs);
}

bool operator!=(const Expression& lhs, const Expression& rhs) {
    return typeid(lhs) != typeid(rhs) || !lhs.is_equal(rhs);
}

std::ostream &operator<<(std::ostream& os, const Expression& exp) {
    exp.print(os);
    return os;                                                      
}

// IdentExpression

IdentExpression::IdentExpression(std::string ident) : Expression(false), ident(ident) {}

Value *IdentExpression::code_gen(Irgen *irgen) const {
    return irgen->read_var(ident, irgen->current_block());
}

bool IdentExpression::is_equal(const Expression &e) const {
    IdentExpression ident_exp = dynamic_cast<const IdentExpression &>(e);
    return ident == ident_exp.ident;
}

void IdentExpression::print(std::ostream &os) const {
    os << ident;
}

// LiteralExpression

LiteralExpression::LiteralExpression(TokenType type, std::string value) : Expression(false), type(type), 
    value(value) {}

Value *LiteralExpression::code_gen(Irgen *irgen) const {
    Const *con = new Const(irgen->next_var_id(), new PrimitiveType(type), value);
    irgen->current_block()->append_instruction(con);
    return static_cast<Value *>(con);
}

bool LiteralExpression::is_equal(const Expression &e) const {
    LiteralExpression literal_exp = dynamic_cast<const LiteralExpression &>(e);
    return type == literal_exp.type && value == literal_exp.value;
}

void LiteralExpression::print(std::ostream &os) const {
    os << value;
}

// UnaryExpression

UnaryExpression::UnaryExpression(TokenType op_type, Expression *exp) : 
    Expression(true), op_type(op_type), exp(exp) {}

Value *UnaryExpression::code_gen(Irgen *irgen) const {
    Value *value = irgen->gen(exp);
    UnaryOp *op = new UnaryOp(irgen->next_var_id(), value, op_type);
    irgen->current_block()->append_instruction(op);
    return static_cast<Value *>(op);
}

bool UnaryExpression::is_equal(const Expression &e) const {
    UnaryExpression unary_exp = dynamic_cast<const UnaryExpression &>(e);
    return op_type == unary_exp.op_type && *exp == *unary_exp.exp;
}

void UnaryExpression::print(std::ostream &os) const {
    os << op_type;
    if (exp->is_bracketed_with_unary) os << "(";
    os << *exp;
    if (exp->is_bracketed_with_unary) os << ")";
}

// BinaryExpression

BinaryExpression::BinaryExpression(TokenType op_type, Expression *lhs, Expression *rhs) 
    : Expression(true), op_type(op_type), lhs(lhs), rhs(rhs) {}

Value *BinaryExpression::code_gen(Irgen *irgen) const {
    Value *lhs_value = irgen->gen(lhs);
    Value *rhs_value = irgen->gen(rhs);
    BinOp *op = new BinOp(irgen->next_var_id(), lhs_value, rhs_value, op_type);
    irgen->current_block()->append_instruction(op);
    return static_cast<Value *>(op);
}

bool BinaryExpression::is_equal(const Expression &e) const {
    BinaryExpression binary_exp = dynamic_cast<const BinaryExpression &>(e);
    return op_type == binary_exp.op_type && *lhs == *binary_exp.lhs && *rhs == *binary_exp.rhs;
}

void BinaryExpression::print(std::ostream &os) const {
    os << *lhs << " " << op_type << " " << *rhs;
}

// CallExpression

CallExpression::CallExpression(std::string function_name, std::vector<Expression *> args)
    : Expression(false), function_name(function_name), args(args) {}

Value *CallExpression::code_gen(Irgen *irgen) const {
    // Generate argument expressions
    std::vector<Value *> arg_values;
    for (Expression *arg : args) arg_values.push_back(irgen->gen(arg));
    
    Call *call_exp = new Call(irgen->next_var_id(), function_name, arg_values);
    irgen->current_block()->append_instruction(call_exp);
    return static_cast<Value *>(call_exp);
}

bool CallExpression::is_equal(const Expression &e) const {
    CallExpression call_exp = dynamic_cast<const CallExpression &>(e);
    if (args.size() != call_exp.args.size()) return false;
    for (int i = 0; i < args.size(); i++) {
        if (*args[i] != *call_exp.args[i]) return false;
    }
    return function_name == call_exp.function_name;
}

void CallExpression::print(std::ostream &os) const {
    os << function_name;
    os << "(";
    int i = 0;
    for (auto a : args) {
        if(i++ > 0) os << ", ";
        os << *a;
    }
    os << ")";
}

// Statement

Statement::Statement(Expression *value) : ret(value), type(RETURN) {}
Statement::Statement(std::vector<Statement *> statements) : block(statements), type(BLOCK) {}
Statement::Statement(Expression *condition, Statement *body, Statement *elses) 
    : ifs{condition, body, elses}, type(IF) { assert(body); }
Statement::Statement(Expression *variable, TokenType type, Expression *value)
    : assign{variable, type, value}, type(ASSIGN) {}
Statement::Statement(Statement *declaration, Expression *condition, Statement *increment, 
    Statement *body) : fors{declaration, condition, increment, body}, type(FOR) {}

bool Statement::is_equal(const Statement &smt) const {
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

bool operator==(const Statement& lhs, const Statement& rhs) {
    return lhs.is_equal(rhs);
}

bool operator!=(const Statement& lhs, const Statement& rhs) {
    return !lhs.is_equal(rhs);
}

std::ostream &operator<<(std::ostream& os, const Statement& smt) {
    switch(smt.type) {
        case Statement::RETURN:
            os << "return " << *smt.ret;
            break;

        case Statement::BLOCK:
            os << "{" << std::endl;
            for (auto s : smt.block) {
                os << "  " << *s << std::endl;
            }
            os << "}" << std::endl;       
            break;

        case Statement::IF:
            if (smt.ifs.condition) os << "if " << *smt.ifs.condition << " ";
            os << *smt.ifs.body;
            if (smt.ifs.elses) os << "else " << *smt.ifs.elses;
            break;

        case Statement::ASSIGN:
            os << *smt.assign.variable << " " << smt.assign.type << " " 
                << *smt.assign.value;
            break;

        case Statement::FOR:
            os << "for " << *smt.fors.declaration << "; " << *smt.fors.condition << "; " 
                << *smt.fors.increment << " " << *smt.fors.body;
            break;
    }
    
    return os;
}

Function::Function(std::string name, std::vector<std::tuple<Type *, std::string>> arguments,
    std::vector<std::tuple<Type *, std::string>> returns, Statement *body) : name(name), 
    arguments(arguments), returns(returns), body(body) {}

bool Function::is_equal(const Function &func) const {
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

std::ostream &operator<<(std::ostream& os, const Function& func) {
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
