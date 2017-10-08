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
    return irgen->read_var(ident, irgen->current_block);
}

bool IdentExpression::is_equal(const Expression &e) const {
    IdentExpression ident_exp = dynamic_cast<const IdentExpression &>(e);
    return ident == ident_exp.ident;
}

void IdentExpression::print(std::ostream &os) const {
    os << ident;
}

// LiteralExpression

LiteralExpression::LiteralExpression(enum TokenType type, std::string value) : Expression(false),
    type(type), value(value) {}

Value *LiteralExpression::code_gen(Irgen *irgen) const {
    Const *con = new Const(irgen->next_var_id(), new PrimitiveType(type), value);
    irgen->current_block->append_instruction(con);
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

UnaryExpression::UnaryExpression(enum TokenType op_type, Expression *exp) :
    Expression(true), op_type(op_type), exp(exp) {}

Value *UnaryExpression::code_gen(Irgen *irgen) const {
    Value *value = irgen->gen(exp);
    UnaryOp *op = new UnaryOp(irgen->next_var_id(), value, op_type);
    irgen->current_block->append_instruction(op);
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

BinaryExpression::BinaryExpression(enum TokenType op_type, Expression *lhs, Expression *rhs)
    : Expression(true), op_type(op_type), lhs(lhs), rhs(rhs) {}

Value *BinaryExpression::code_gen(Irgen *irgen) const {
    Value *lhs_value = irgen->gen(lhs);
    Value *rhs_value = irgen->gen(rhs);
    BinOp *op = new BinOp(irgen->next_var_id(), lhs_value, rhs_value, op_type);
    irgen->current_block->append_instruction(op);
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
    irgen->current_block->append_instruction(call_exp);
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

Statement::Statement() {}

bool operator==(const Statement& lhs, const Statement& rhs) {
    return typeid(lhs) == typeid(rhs) && lhs.is_equal(rhs);
}

bool operator!=(const Statement& lhs, const Statement& rhs) {
    return typeid(lhs) != typeid(rhs) || !lhs.is_equal(rhs);
}

std::ostream& operator<<(std::ostream& os, const Statement& smt) {
    smt.print(os);
    return os;
}

ReturnStatement::ReturnStatement(Expression *exp) : Statement(), exp(exp) {}

void ReturnStatement::code_gen(Irgen *irgen) const {
    Value *value = irgen->gen(exp);
    Instruction *ins = new Ret(value);
    irgen->current_block->append_instruction(ins);
}

bool ReturnStatement::is_equal(const Statement &s) const {
    ReturnStatement return_smt = dynamic_cast<const ReturnStatement &>(s);
    return *exp == *return_smt.exp;
}

void ReturnStatement::print(std::ostream &os) const {
    os << "return " << *exp;
}

// Block Statement

BlockStatement::BlockStatement(std::vector<Statement *> statements) : statements(statements) {}

void BlockStatement::code_gen(Irgen *irgen) const {
    BasicBlock *new_block = irgen->new_basic_block();
    BasicBlock *out_block = irgen->gen_block(new_block, this);
    BasicBlock *continue_block = irgen->new_basic_block();
    
    irgen->current_block->append_instruction(new Branch(new_block, irgen->current_block));
    // TODO: should this be current_block or out_block? 
    out_block->append_instruction(new Branch(continue_block, irgen->current_block));
    
    irgen->current_block = continue_block;
}

bool BlockStatement::is_equal(const Statement &s) const {
    BlockStatement block_smt = dynamic_cast<const BlockStatement &>(s);

    if (statements.size() != block_smt.statements.size()) return false;
    for (size_t i = 0; i < block_smt.statements.size(); i++) {
        if(!(*statements[i] == *block_smt.statements[i])) return false;
    }
    return true;
}

void BlockStatement::print(std::ostream &os) const {
    os << "{" << std::endl;
    for (auto s : statements) {
        os << "  " << *s << std::endl;
    }
    os << "}" << std::endl;       
}

// If Statement

IfStatement::IfStatement(Expression *condition, BlockStatement *body, IfStatement *elses) : 
    condition(condition), body(body), elses(elses) {}

void IfStatement::code_gen(Irgen *irgen) const {
    BasicBlock *end_block = irgen->new_basic_block();
    code_gen_if_branch(irgen, this, nullptr, end_block);
}

void IfStatement::code_gen_if_branch(Irgen *irgen, const IfStatement *ifs, BasicBlock *parent_block, 
    BasicBlock *end_block) const {

    if(!ifs->condition) {
        // Statment is an else block, gen and exit
        BasicBlock *out_block = irgen->gen_block(parent_block, ifs->body);
        
        // If "else" doesnt terminate, branch to end block
        if (!out_block->is_terminated()) {
            out_block->append_instruction(new Branch(end_block, out_block));
            irgen->current_block = end_block;
        } else {
            irgen->current_block = out_block;
            irgen->destroy_basic_block(end_block->id);
        }
        
        return;
    }

    // Generate conditional
    Value *condition = irgen->gen(ifs->condition);
    
    // If we wernt passed a parent block, assume current block
    if (parent_block == nullptr) parent_block = irgen->current_block;

    // true block for if the conditional is true
    BasicBlock *true_block = irgen->new_basic_block();

    // false block is either the next else/elseif block if the block to continue from
    BasicBlock *false_block = end_block;
    if(ifs->elses) false_block = irgen->new_basic_block();
        
    // Branch into "if branch" from parent
    parent_block->append_instruction(
        new ConditionalBranch(true_block, false_block, parent_block, condition));
            
    // generate the true block
    BasicBlock *out_block = irgen->gen_block(true_block, ifs->body);
    if (!out_block->is_terminated())
        out_block->append_instruction(new Branch(end_block, out_block));
        
    // TODO: do we need this?
    irgen->current_block = false_block;

    // Generate chaining elseif/else statements
    if (ifs->elses)
        code_gen_if_branch(irgen, ifs->elses, false_block, end_block);
        
    irgen->current_block = end_block;
}

bool IfStatement::is_equal(const Statement &s) const {
    IfStatement if_smt = dynamic_cast<const IfStatement &>(s);

    return (condition == if_smt.condition || *condition == *if_smt.condition) &&
        (elses == if_smt.elses || *elses == *if_smt.elses) &&
        (body == if_smt.body || *body == *if_smt.body);
}

void IfStatement::print(std::ostream &os) const {
    if (condition) os << "if " << *condition << " ";
    os << *body;
    if (elses) os << "else " << *elses;
}

// Assign Statement

AssignStatement::AssignStatement(Expression *variable, enum TokenType op_type, Expression *exp) :
    variable(variable), op_type(op_type), exp(exp) {}

void AssignStatement::code_gen(Irgen *irgen) const {
    std::string var_name = static_cast<const IdentExpression *>(variable)->ident;
    Value *value = irgen->gen(exp);

    // TODO: make variable of type IdentExpression
    if(op_type == TokenType::SYMBOL_DEFINE || op_type == TokenType::SYMBOL_ASSIGN) {
        irgen->write_var(var_name, value);
        return;
    }

    Value *var_value = irgen->read_var(var_name, irgen->current_block);
    enum TokenType bin_op;

    switch(op_type) {
        case TokenType::SYMBOL_ADD_ASSIGN: {
            bin_op = TokenType::SYMBOL_ADD;
            break;
        }
        case TokenType::SYMBOL_SUB_ASSIGN: {
            bin_op = TokenType::SYMBOL_SUB;
            break;
        }
        case TokenType::SYMBOL_MUL_ASSIGN: {
            bin_op = TokenType::SYMBOL_MUL;
            break;
        }
        case TokenType::SYMBOL_QUO_ASSIGN: {
            bin_op = TokenType::SYMBOL_QUO;
            break;
        }
        case TokenType::SYMBOL_REM_ASSIGN: {
            bin_op = TokenType::SYMBOL_REM;
            break;
        }
        default: {
            assert(false);
            break;
        }
    }

    BinOp *op = new BinOp(irgen->next_var_id(), var_value, value, bin_op);
    irgen->current_block->append_instruction(op);
    irgen->write_var(var_name, op);
}

bool AssignStatement::is_equal(const Statement &s) const {
    AssignStatement assign_smt = dynamic_cast<const AssignStatement &>(s);
    return *variable == *assign_smt.variable &&
        op_type == assign_smt.op_type &&
        *exp == *assign_smt.exp;
}

void AssignStatement::print(std::ostream &os) const {
    os << *variable << " " << op_type << " " << *exp;
}

// For Statement

ForStatement::ForStatement(Statement *declaration, Expression *condition, Statement *increment, 
    BlockStatement *body) : declaration(declaration), condition(condition), increment(increment),
    body(body) {}

void ForStatement::code_gen(Irgen *irgen) const {
    // Generate for loop body
    BasicBlock *in_for_block = irgen->new_basic_block();
	in_for_block->append_pred(irgen->current_block);
    BasicBlock *out_for_block = irgen->gen_block(in_for_block, body);
	in_for_block->append_pred(out_for_block);

	// Create continue block
    BasicBlock *continue_block = irgen->new_basic_block();

    // Conditionaly branch into for loop
    irgen->gen(declaration);
    Value *value = irgen->gen(condition);
    irgen->current_block->append_instruction(new ConditionalBranch(in_for_block, continue_block,
        irgen->current_block, value));

    // Conditionly continue for loop
    // TODO: Added target block to gen and make current_block read only
    irgen->current_block = out_for_block;
    irgen->gen(increment);
    Value *inner_value = irgen->gen(condition);
    out_for_block->append_instruction(new ConditionalBranch(in_for_block, continue_block,
        out_for_block, inner_value));

    irgen->current_block = continue_block;
}

bool ForStatement::is_equal(const Statement &s) const {
    ForStatement for_smt = dynamic_cast<const ForStatement &>(s);
    return *declaration == *for_smt.declaration &&
        *condition == *for_smt.condition &&
        *increment == *for_smt.increment &&
        *body == *for_smt.body;
}

void ForStatement::print(std::ostream &os) const {
    os << "for " << *declaration << "; " << *condition << "; " << *increment << " " << *body;
}

// Function

Function::Function(std::string name, std::vector<std::tuple<Type *, std::string>> arguments,
    std::vector<std::tuple<Type *, std::string>> returns, BlockStatement *body) : name(name), 
    arguments(arguments), returns(returns), body(body) {}

bool Function::is_equal(const Function &func) const {
    if(name != func.name) return false;
    
    // Check arguments are equal
    if(arguments.size() != func.arguments.size()) return false;
    for (size_t i = 0; i < arguments.size(); i++) {
        auto type = std::get<0>(arguments[i]);
        auto func_type = std::get<0>(func.arguments[i]);
        if(*type != *func_type) return false;

        auto name = std::get<1>(arguments[i]);
        auto func_name = std::get<1>(func.arguments[i]);
        if(name != func_name) return false;
    }
    
    // Check returns are equal
    if(returns.size() != func.returns.size()) return false;
    for (size_t i = 0; i < returns.size(); i++) {
        auto type = std::get<0>(returns[i]);
        auto func_type = std::get<0>(func.returns[i]);
        if(*type != *func_type) return false;

        auto name = std::get<1>(returns[i]);
        auto func_name = std::get<1>(func.returns[i]);
        if(name != func_name) return false;
    }

    // Check bodys are equal
    if(*body != *func.body) return false;
    
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
