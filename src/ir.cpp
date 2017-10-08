// Value

Value::Value(int id, Type *type) : id(id), type(type) {}

// TODO: rename this to print_value
void Value::printValue(std::ostream& os) const {
    os << "v" << id /*<< " : " << *val.type*/;
}

std::ostream& operator<<(std::ostream& os, const Value& val) {
    val.printValue(os);
    return os;                                                         
}

// Instruction

Instruction::Instruction (bool terminatator) : terminatator(terminatator) {}

bool Instruction::is_terminator() {
    return terminatator;
}

void Instruction::print_instruction(std::ostream& os) const {
    os << "[INVALID INSTRUCTION]";
};

std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
    ins.print_instruction(os);
    return os;                                                         
}

// BasicBlock

BasicBlock::BasicBlock(int id) :id(id), instructions({}), terminatator(nullptr) {}

void BasicBlock::append_instruction(Instruction *i) {
    instructions.push_back(i);
    terminatator = i;
    return;
}

void BasicBlock::append_pred(BasicBlock *block) {
    if(std::find(preds.begin(), preds.end(), block) == preds.end())
        preds.push_back(block);
}

bool BasicBlock::is_terminated() {
    if(!terminatator) return false;
    return terminatator->is_terminator();
}

std::string BasicBlock::to_string() const {
    std::ostringstream stream;
    stream << "b" << id << ":";
#if 0
    stream << " (";
    for (int i = 0; i < preds.size(); i++) {
        stream << "b" << preds[i]->id;
        if (i < preds.size() - 1) {
            stream << ", ";
        }
    }
    stream << ")";
#endif
    stream << std::endl;
    for (auto i : instructions) {
        stream << "\t" << *i << std::endl;
    }

    return stream.str();;
}

std::ostream& operator<<(std::ostream& os, const BasicBlock &block) {
    os << block.to_string();
    return os;
}

// IrFunction

IrFunction::IrFunction(
    std::string name,
    std::vector<std::tuple<Type *, std::string>> arguments,
    std::vector<std::tuple<Type *, std::string>> returns) : 
        name(name), arguments(arguments), returns(returns) {}



void IrFunction::append_block(BasicBlock *block) {
    blocks.push_back(block);
}

std::string IrFunction::to_string() const {
    std::ostringstream stream;
    
    stream << "proc " << name << " :: ";
    
    for (size_t i = 0; i < arguments.size(); i++) {
        auto arg = arguments[i];
        if(i > 0) stream << ", ";
        stream << *std::get<0>(arg) << " " << std::get<1>(arg);
    }
    
    if(arguments.size() != 0) stream << " ";
    
    stream << "-> ";
    
    for (size_t i = 0; i < returns.size(); i++) {
        auto ret = returns[i];
        if(i > 0) stream << ", ";
        stream << *std::get<0>(ret) << " " << std::get<1>(ret);
    }

    stream << ":" << std::endl;
    
    for(int i = 0; i < blocks.size(); i++) {
        stream << *blocks[i];
        if (i < blocks.size()-1) stream << std::endl;
    }

    std::string str = stream.str();
    return str;
}

std::ostream& operator<<(std::ostream& os, const IrFunction& func) {
    os << func.to_string();
    return os;
}

// Const

Const::Const(int id, Type *type, std::string value) : Instruction(false), Value(id, type), 
    value(value) {}

void Const::print_instruction(std::ostream &os) const {
    os << static_cast<Value>(*this) << " = " << value;
}

std::ostream &operator<<(std::ostream& os, const Const &con) {
    con.print_instruction(os);
    return os;                                                         
}

// Arg

Arg::Arg(int id, Type *type, std::string name) : Instruction(false), Value(id, type), name(name) {}

void Arg::print_instruction(std::ostream& os) const {
    os << static_cast<Value>(*this) << " = " << "argument " << name;
}

std::ostream &operator<<(std::ostream& os, const Arg& arg) {
    arg.printValue(os);
    return os;   
}

// Call

Call::Call(int id, std::string function_name, std::vector<Value *> args) : Instruction(false), 
    Value(id, NULL), function_name(function_name), args(args) {}

void Call::print_instruction(std::ostream& os) const {
    os << static_cast<Value>(*this) << " = " << function_name << "(";
    for (int i = 0; i < args.size(); i++) {
        os << *args[i];
        if (i < args.size() - 1) os << ", ";
    }
    os << ")";
}

std::ostream& operator<<(std::ostream& os, const Call& call) {
    call.print_instruction(os);
    return os;                                        
}

// BinOp

BinOp::BinOp(int id, Value *lhs, Value *rhs, enum TokenType op)
    : Instruction(false), Value(id, lhs->type), lhs(lhs), rhs(rhs), op(op) {}

void BinOp::print_instruction(std::ostream& os) const {
    os << static_cast<Value>(*this) << " = " << *lhs << " " << op << " " << *rhs;
}

std::ostream& operator<<(std::ostream& os, const BinOp& bin_op) {
    bin_op.print_instruction(os);
    return os;                                                         
}

// UnaryOp

UnaryOp::UnaryOp(int id, Value *value, enum TokenType op) : Instruction(false), Value(id, value->type),
    value(value), op(op) {}

void UnaryOp::print_instruction(std::ostream& os) const {
    os << static_cast<Value>(*this) << " = " << op << *value;
}

std::ostream& operator<<(std::ostream& os, const UnaryOp& unary_op) {
    unary_op.print_instruction(os);
    return os;                                                         
}

// Ret

Ret::Ret(Value *val) : Instruction(true), val(val) {}

void Ret::print_instruction(std::ostream& os) const {
    os << "ret " << *val;
}

std::ostream& operator<<(std::ostream& os, const Ret& ret) {
    ret.print_instruction(os);
    return os;                                        
}

// Branch

Branch::Branch(BasicBlock *destination, BasicBlock *source) : Instruction(true), 
    destination(destination) {
    
    destination->append_pred(source);
}

void Branch::print_instruction(std::ostream &os) const {
    os << "br b" << destination->id; 
}

std::ostream &operator<<(std::ostream &os, const Branch &branch) {
    branch.print_instruction(os);
    return os;
}

// ConditionalBranch

ConditionalBranch::ConditionalBranch(BasicBlock *true_block, BasicBlock *false_block, 
    BasicBlock *source_block, Value *condition) : Instruction(true), true_block(true_block), 
    false_block(false_block), condition(condition) {

    true_block->append_pred(source_block);
    false_block->append_pred(source_block);
}

void ConditionalBranch::print_instruction(std::ostream &os) const {
    os << "condbr " << *condition 
        << " b" << true_block->id 
        << " b" << false_block->id;
}

std::ostream &operator<<(std::ostream &os, const ConditionalBranch &branch) {
    branch.print_instruction(os);
    return os;
}

// Phi

PhiOperand::PhiOperand(int block_id, Value *value) : block_id(block_id), value(value) {}

Phi::Phi(int id, std::vector<PhiOperand> operands) : Instruction(false), Value(id, nullptr), 
    operands(operands) {}

void Phi::print_instruction(std::ostream &os) const {
    if (type == nullptr) {
        os << static_cast<Value>(*this) << " = phi null";
        return;
    }

    os << static_cast<Value>(*this) << " = phi ";

    for(int i = 0; i < operands.size(); i++) {
        os << "(b" << operands[i].block_id << " : " << *operands[i].value << ")";
        if (i < operands.size() - 1) os << " ";
    }
}

void Phi::append_operand(const PhiOperand &op) {
    assert(type == nullptr || *type == *op.value->type);
    type = op.value->type;
    operands.push_back(op);
}

std::ostream &operator<<(std::ostream &os, const Phi &phi) {
    phi.print_instruction(os);
    return os;
}