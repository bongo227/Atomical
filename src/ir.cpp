struct Instruction {};

struct BasicBlock {
    std::vector<Instruction> instructions;

    BasicBlock(std::vector<Instruction> instructions) : instructions(instructions) {}
};

struct IrFunction {
    std::vector<BasicBlock> basic_blocks;

    IrFunction(std::vector<BasicBlock> basic_blocks) : basic_blocks(basic_blocks) {}
};

struct Variable {
    int id;
    Type *type;

    Variable(int id, Type *type) : id(id), type(type) {}
};

struct RetInstruction : Instruction {
    Variable *variable;

    RetInstruction(Variable *variable) : variable(variable) {}
};

struct BrInstruction : Instruction {
    BasicBlock *destination;

    BrInstruction(BasicBlock *destination) : destination(destination) {}
};

struct AddInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    AddInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct SubInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    SubInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct MulInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    MulInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct DivInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;
    bool iunsigned;

    DivInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct ShiftLeftInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    ShiftLeftInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct ShiftRightInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;
    bool iunsigned;

    ShiftRightInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct AndInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    AndInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct OrInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    OrInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct XorInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;

    XorInstruction(Variable *lhs, Variable *rhs) : lhs(lhs), rhs(rhs) {}
};

struct CastInstruction : Instruction {
    Variable *var;
    Type *cast_type;

    CastInstruction(Variable *var, Type *cast_type) : var(var), cast_type(cast_type) {}
};

enum class CompareType {
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqualTo,
    MoreThanOrEqualTo
};

struct CompareInstruction : Instruction {
    Variable *lhs;
    Variable *rhs;
    CompareType type;
    bool iordered;
    bool iunsigned;

    CompareInstruction(Variable *lhs, Variable *rhs, CompareType type, bool iordered, 
        bool iunsigned) : lhs(lhs), rhs(rhs), type(type), iordered(iordered), iunsigned(iunsigned) 
        {}
};

struct PhiInstruction : Instruction {
    std::vector<std::tuple<BasicBlock *, Variable *>> incoming;

    PhiInstruction(std::vector<std::tuple<BasicBlock *, Variable *>> incoming) : incoming(incoming)
        {}
};

struct CallInstruction : Instruction {
    Function *function;
    std::vector<Variable *> arguments;

    CallInstruction(Function *function, std::vector<Variable *> arguments) : function(function),
        arguments(arguments) {}
};
