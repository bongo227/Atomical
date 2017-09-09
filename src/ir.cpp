struct Instruction {
    Instruction () {}

    virtual void print_instruction(std::ostream& os) const {
        os << "[INVALID INSTRUCTION]";
    };

    friend std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
        ins.print_instruction(os);
        return os;                                                         
    }
};

struct BasicBlock {
    int id;
    std::vector<Instruction *> instructions;
    Instruction *terminatator;

    BasicBlock(int id) :id(id), instructions({}), terminatator(nullptr) {}

    void append_instruction(Instruction *i) {
        instructions.push_back(i);
        terminatator = i;
        return;
    }

    std::string to_string() const {
        std::ostringstream stream;
        stream << "b" << id << ":" << std::endl;
        for (auto i : instructions) {
            stream << "\t" << *i << std::endl;
        }

        return stream.str();;
    }

    friend std::ostream& operator<<(std::ostream& os, const BasicBlock& block) {
        os << block.to_string();
        return os;
    }
};

struct IrFunction {
    std::string name;
    std::vector<std::tuple<Type *, std::string>> arguments;
    std::vector<std::tuple<Type *, std::string>> returns;
    std::vector<BasicBlock> blocks;

    IrFunction(
        std::string name,
        std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns) : 
            name(name), arguments(arguments), returns(returns) {}

    void append_block(BasicBlock block) {
        blocks.push_back(block);
    }

    std::string to_string() const {
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
            stream << blocks[i];
            if (i < blocks.size()-1) stream << std::endl;
        }

        std::string str = stream.str();
        return str;
    }

    friend std::ostream& operator<<(std::ostream& os, const IrFunction& func) {
        os << func.to_string();
        return os;
    }
};

struct Value {
    int id;
    Type *type;

    Value(int id, Type *type) : id(id), type(type) {}

    virtual void printValue(std::ostream& os) const {
        os << "v" << id /*<< " : " << *val.type*/;
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& val) {
        val.printValue(os);
        return os;                                                         
    }
};

struct Const : public Instruction, public Value {
    std::string value;

    Const(int id, Type *type, std::string value) 
        : Instruction(), Value(id, type), value(value) {}

    void print_instruction(std::ostream &os) const {
        os << static_cast<Value>(*this) << " = " << value;
    }

    friend std::ostream &operator<<(std::ostream& os, const Const& con) {
        con.print_instruction(os);
        return os;                                                         
    }
};

struct Arg : public Instruction, public Value {
    std::string name;

    Arg(int id, Type *type, std::string name) : Instruction(), Value(id, type), name(name) {}

    void print_instruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << "argument " << name;
    }

    friend std::ostream &operator<<(std::ostream& os, const Arg& arg) {
        arg.printValue(os);
        return os;   
    }
};

struct Call : public Instruction, public Value {
    std::string function_name;
    std::vector<Value *> args;

    Call(int id, std::string function_name, std::vector<Value *> args) 
        : Value(id, NULL), function_name(function_name), args(args) {}

    void print_instruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << function_name << "(";
        for (int i = 0; i < args.size(); i++) {
            os << *args[i];
            if (i < args.size() - 1) os << ", ";
        }
        os << ")";
    }

    friend std::ostream& operator<<(std::ostream& os, const Call& call) {
        call.print_instruction(os);
        return os;                                        
    }
};

struct BinOp : public Instruction, public Value {
    Value *lhs;
    Value *rhs;
    TokenType op;

    BinOp(int id, Value *lhs, Value *rhs, TokenType op)
        : Instruction(), Value(id, lhs->type), lhs(lhs), rhs(rhs), op(op) {
        // assert(*lhs.type == *rhs.type);
    }

    void print_instruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << *lhs << " " << op << " " << *rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const BinOp& bin_op) {
        bin_op.print_instruction(os);
        return os;                                                         
    }
};

struct UnaryOp : public Instruction, public Value {
    Value *value;
    TokenType op;

    UnaryOp(int id, Value *value, TokenType op) 
        : Instruction(), Value(id, value->type), value(value), op(op) {}

    void print_instruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << op << *value;
    }

    friend std::ostream& operator<<(std::ostream& os, const UnaryOp& unary_op) {
        unary_op.print_instruction(os);
        return os;                                                         
    }
};

struct Ret : public Instruction {
    Value *val;
    
    Ret(Value *val) : Instruction(), val(val) {}

    void print_instruction(std::ostream& os) const {
        os << "ret " << *val;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ret& ret) {
        ret.print_instruction(os);
        return os;                                        
    }
};

