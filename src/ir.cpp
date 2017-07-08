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

struct Value {
    TokenType type;
    std::string *value;

    Value(TokenType type, std::string value) : type(type), value(&value) {}
};

struct Parameter{
    union {
        Variable var;
        Value val;
    };

    enum { VARIABLE, VALUE } type;

    Parameter(Variable var) : var(var), type(VARIABLE) {}
    Parameter(Value val) : val(val), type(VALUE) {}
    Parameter(int id, Type *type) : var(Variable(id, type)), type(VARIABLE) {}
    Parameter(TokenType type, std::string value) : val(Value(type, value)), type(VALUE) {}
};

struct RetInstruction : Instruction {
    Parameter *value;

    RetInstruction(Parameter *value) : value(value) {}
};

struct BrInstruction : Instruction {
    BasicBlock *destination;

    BrInstruction(BasicBlock *destination) : destination(destination) {}
};

struct AddInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    AddInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct SubInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    SubInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct MulInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    MulInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct DivInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;
    bool iunsigned;

    DivInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct ShiftLeftInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    ShiftLeftInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct ShiftRightInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;
    bool iunsigned;

    ShiftRightInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct AndInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    AndInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct OrInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    OrInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct XorInstruction : Instruction {
    Parameter *lhs;
    Parameter *rhs;

    XorInstruction(Parameter *lhs, Parameter *rhs) : lhs(lhs), rhs(rhs) {}
};

struct CastInstruction : Instruction {
    Parameter *var;
    Type *cast_type;

    CastInstruction(Parameter *var, Type *cast_type) : var(var), cast_type(cast_type) {}
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
    Parameter *lhs;
    Parameter *rhs;
    CompareType type;
    bool iordered;
    bool iunsigned;

    CompareInstruction(Parameter *lhs, Parameter *rhs, CompareType type, bool iordered, 
        bool iunsigned) : lhs(lhs), rhs(rhs), type(type), iordered(iordered), iunsigned(iunsigned) 
        {}
};

struct PhiInstruction : Instruction {
    std::vector<std::tuple<BasicBlock *, Parameter>> incoming;

    PhiInstruction(std::vector<std::tuple<BasicBlock *, Parameter>> incoming) : incoming(incoming)
        {}
};

struct CallInstruction : Instruction {
    Function *function;
    std::vector<Parameter> arguments;

    CallInstruction(Function *function, std::vector<Parameter> arguments) : function(function),
        arguments(arguments) {}
};
