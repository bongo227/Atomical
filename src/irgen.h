class Function;
class Statement;
class BlockStatement;
class Expression;

class Irgen {
    std::vector<Function *> functions;
    Function *current_source_function;
    IrFunction *current_ir_function;
    std::map<std::string, std::map<int, Value *>> current_def;
    
    int var_id_counter;
    int block_id_counter;

public:
    BasicBlock *current_block;
    
    explicit Irgen(std::vector<Function *> functions);
    explicit Irgen(std::string source);

    void write_var(std::string var_name, int block_id, Value *value);
    void write_var(std::string var_name, Value *value);
    Value *read_var(std::string var_name, BasicBlock *block);
    Value *read_var_recursive(std::string var_name, BasicBlock *block);
    void add_phi_operands(std::string var_name, BasicBlock *block, Phi *phi);

    int next_var_id() { return ++var_id_counter; }
    int next_block_id() { return ++block_id_counter; }

    BasicBlock *new_basic_block();
    void destroy_basic_block(int block_id);
    
    Value *gen(const Expression *exp);
    void gen_if_branch(const Statement *smt, BasicBlock *parent_block, BasicBlock *end_block);
    BasicBlock *gen_block(BasicBlock *target_block, const BlockStatement *block);
    IrFunction *gen(Function *func);
    void gen(const Statement *smt);
    std::vector<IrFunction *> gen(const std::vector<Function *> funcs);
    
    std::string to_string();
};