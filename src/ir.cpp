struct Instruction;

struct BasicBlock {
    int id;
    std::vector<Instruction> instructions;

    BasicBlock(std::vector<Instruction> instructions) : instructions(instructions) {}
};

struct IrFunction {
    std::vector<BasicBlock> basic_blocks;

    IrFunction(std::vector<BasicBlock> basic_blocks) : basic_blocks(basic_blocks) {}
};

struct Parameter{
    union {
        int variable;
        std::string value;
    };
    Type *type;

    enum { VARIABLE, VALUE, NONE } parameter_type;

    Parameter(int id, Type *type) : variable(id), type(type), parameter_type(VARIABLE) {}
    Parameter(std::string value, Type *type) : value(value), type(type), parameter_type(VALUE) {}
    Parameter() : parameter_type(NONE) {}

    Parameter(const Parameter &param) {
        switch(param.parameter_type) {
            case VARIABLE: Parameter(param.variable, param.type); break;
            case VALUE: Parameter(param.value, param.type); break;
            case NONE: assert(false);
        }
    }

    Parameter &operator=(const Parameter &param) {
        switch(param.parameter_type) {
            case VARIABLE: variable = param.variable;
            case VALUE: value = param.value;
            case NONE: assert(false);
        }
        type = param.type;
        parameter_type = param.parameter_type;
    }

    ~Parameter() {
        if(parameter_type == NONE) assert(false);
        if(parameter_type == VALUE) value.std::string::~string();
    }
};

enum class CompareType {
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqualTo,
    MoreThanOrEqualTo
};

struct Instruction {
    typedef enum {
        RETURN, 
        BRANCH, 
        ADD, 
        SUB, 
        MUL, 
        DIV, 
        SHIFT_LEFT, 
        SHIFT_RIGHT, 
        AND, 
        OR, 
        XOR, 
        CAST, 
        COMPARE, 
        PHI 
    } InstructionType;

    Parameter param;
    InstructionType instruction_type;
    
    union {
        Parameter ret;
        BasicBlock *br;
        struct { Parameter lhs; Parameter rhs; } binary;
        struct { Type *type; Parameter value; } cast;
        std::vector<std::tuple<BasicBlock *, Parameter>> phi;
    };


    // TODO: collapse into flags
    // CompareType compare_type;
    // bool instruction_ordered;
    // bool instruction_unsigned;

    private:
        Instruction(Parameter param, Parameter value) : 
            param(param), ret(value), instruction_type(InstructionType::RETURN) {}
        Instruction(Parameter param, BasicBlock *destination) : 
            param(param), br(destination), instruction_type(InstructionType::BRANCH) {}
        Instruction(Parameter param, Parameter lhs, Parameter rhs, InstructionType type) : 
            param(param), binary{lhs, rhs}, instruction_type(type) {}
        Instruction(Parameter param, Type *type, Parameter value) : 
            param(param), cast{type, value}, instruction_type(InstructionType::CAST) {}
        Instruction(Parameter param, std::vector<std::tuple<BasicBlock *, Parameter>> incoming) : 
            param(param), phi(incoming) {}

    public:
        static Instruction Return(Parameter param, Parameter value) 
            { return Instruction(param, value); }
        static Instruction Branch(Parameter param, BasicBlock *destination) 
            { return Instruction(param, destination); }
        static Instruction Add(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::ADD); }
        static Instruction Sub(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::SUB); }
        static Instruction Mul(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::MUL); }
        static Instruction Div(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::DIV); }
        static Instruction ShiftLeft(Parameter param, Parameter lhs, Parameter rhs) 
            { return Instruction(param, lhs, rhs, InstructionType::SHIFT_LEFT); }
        static Instruction ShiftRight(Parameter param, Parameter lhs, Parameter rhs) 
            { return Instruction(param, lhs, rhs, InstructionType::SHIFT_RIGHT); }
        static Instruction And(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::AND); }
        static Instruction Or(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::OR); }
        static Instruction Xor(Parameter param, Parameter lhs, Parameter rhs)
            { return Instruction(param, lhs, rhs, InstructionType::XOR); }
        static Instruction Cast(Parameter param, Type *type, Parameter value) 
            { return Instruction(param, type, value); }
        static Instruction Phi(Parameter param, std::vector<std::tuple<BasicBlock *, Parameter>> incoming)
            { return Instruction(param, incoming); }

    Instruction(const Instruction &inst) {
        switch(inst.instruction_type) {
            case InstructionType::RETURN: Instruction(inst.param, inst.ret); break; 
            case InstructionType::BRANCH: Instruction(inst.param, inst.br); break; 
            case InstructionType::ADD: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::ADD); break; 
            case InstructionType::SUB: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::SUB); break; 
            case InstructionType::MUL: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::MUL); break; 
            case InstructionType::DIV: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::DIV); break; 
            case InstructionType::SHIFT_LEFT: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::SHIFT_LEFT); break; 
            case InstructionType::SHIFT_RIGHT: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::SHIFT_RIGHT); break; 
            case InstructionType::AND: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::AND); break; 
            case InstructionType::OR: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::OR); break; 
            case InstructionType::XOR: Instruction(inst.param, inst.binary.lhs, inst.binary.rhs, 
                InstructionType::XOR); break; 
            case InstructionType::CAST: Instruction(inst.param, inst.cast.type, inst.cast.value); break; 
            // case COMPARE: Instruction(inst.param, ); break; 
            case InstructionType::PHI: Instruction(inst.param, inst.phi); break;
        }
    }

    ~Instruction() {
        if(instruction_type == InstructionType::PHI) {
            assert(false); // destruct vector;
        }
    }
};