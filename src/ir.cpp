static int idCounter;
struct Value {
    int id;
    Type *type;

    Value(Type *type) : id(++idCounter), type(type) {}
    Value(int id, Type *type) : id(id), type(type) {}

    virtual void printValue(std::ostream& os) const {
        os << "v" << id /*<< " : " << *val.type*/;
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& val) {
        os << "b" << (val.id);
        // val.printValue(os);
        return os;                                                         
    }
};

struct Const : public Value {
    std::string value;

    Const(Type *type, std::string value) : Value(-1, type), value(value) {}

    // void printValue(std::ostream& os) const {
    //     os << value;
    // }

    friend std::ostream& operator<<(std::ostream& os, const Const& con) {
        con.printValue(os);
        return os;                                                         
    }
};

struct Instruction {
    Instruction () {}

    virtual void printInstruction(std::ostream& os) const {
        os << "[INVALID INSTRUCTION]";
    };

    friend std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
        ins.printInstruction(os);
        return os;                                                         
    }
};

struct BinOp : public Instruction, public Value {
    Value *lhs;
    Value *rhs;
    TokenType op;

    BinOp(Value *lhs, Value *rhs, TokenType op)
        : Instruction(), Value(lhs->type), lhs(lhs), rhs(rhs), op(op) {
        // assert(*lhs.type == *rhs.type);
    }

    void printInstruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << *lhs << " " << op << " " << *rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const BinOp& bin_op) {
        bin_op.printInstruction(os);
        return os;                                                         
    }
};

struct UnaryOp : public Instruction, public Value {
    Value *value;
    TokenType op;

    UnaryOp(Value *value, TokenType op) : Instruction(), Value(value->type), op(op) {}

    void printInstruction(std::ostream& os) const {
        os << static_cast<Value>(*this) << " = " << op << *value;
    }

    friend std::ostream& operator<<(std::ostream& os, const UnaryOp& unary_op) {
        unary_op.printInstruction(os);
        return os;                                                         
    }
};

struct Ret : public Instruction {
    Value *val;
    
    Ret(Value *val) : Instruction(), val(val) {}

    void printInstruction(std::ostream& os) const {
        os << "ret " << *val;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ret& ret) {
        ret.printInstruction(os);
        return os;                                                         
    }
};

