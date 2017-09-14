struct Value {
    int id;
    Type *type;

    Value(int id, Type *type);

    virtual void printValue(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Value& val);
};

struct Instruction {
    bool terminatator;

    Instruction (bool terminatator);

    bool is_terminator();
    virtual void print_instruction(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Instruction& ins);
};

struct BasicBlock {
    int id;
    std::vector<BasicBlock *> preds;
    std::vector<Instruction *> instructions;
    Instruction *terminatator;

    BasicBlock(int id);
    
    void append_instruction(Instruction *i);
    bool is_terminated();
    std::string to_string() const;
    friend std::ostream& operator<<(std::ostream& os, const BasicBlock& block);
};

struct IrFunction {
    std::string name;
    std::vector<std::tuple<Type *, std::string>> arguments;
    std::vector<std::tuple<Type *, std::string>> returns;
    std::vector<BasicBlock *> blocks;

    IrFunction(std::string name, std::vector<std::tuple<Type *, std::string>> arguments,
        std::vector<std::tuple<Type *, std::string>> returns);

    void append_block(BasicBlock *block);
    std::string to_string() const;
    friend std::ostream& operator<<(std::ostream& os, const IrFunction& func);
};

struct Const : public Instruction, public Value {
    std::string value;

    Const(int id, Type *type, std::string value);
    
    void print_instruction(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream& os, const Const& con);
};

struct Arg : public Instruction, public Value {
    std::string name;

    Arg(int id, Type *type, std::string name);
    
    void print_instruction(std::ostream& os) const;
    friend std::ostream &operator<<(std::ostream& os, const Arg& arg);
};

struct Call : public Instruction, public Value {
    std::string function_name;
    std::vector<Value *> args;

    Call(int id, std::string function_name, std::vector<Value *> args);
    
    void print_instruction(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Call& call);
};

struct BinOp : public Instruction, public Value {
    Value *lhs;
    Value *rhs;
    TokenType op;

    BinOp(int id, Value *lhs, Value *rhs, TokenType op);

    void print_instruction(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const BinOp& bin_op);
};

struct UnaryOp : public Instruction, public Value {
    Value *value;
    TokenType op;

    UnaryOp(int id, Value *value, TokenType op);

    void print_instruction(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const UnaryOp& unary_op);
};

struct Ret : public Instruction {
    Value *val;
    
    Ret(Value *val);

    void print_instruction(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Ret& ret);
};

struct Branch : public Instruction {
    int block_id;

    Branch(int block_id);
    
    void print_instruction(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const Branch &branch);
};

struct ConditionalBranch : public Instruction {
    int true_block_id;
    int false_block_id;
    Value *condition;

    ConditionalBranch(int true_block_id, int false_block_id, Value *condition);
    
    void print_instruction(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const ConditionalBranch &branch);
};