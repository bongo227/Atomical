class Irgen {
    private:
        std::vector<Function *> functions;
        BasicBlock *current_block;
        Function *current_source_function;
        IrFunction *current_ir_function;

        std::map<std::string, std::map<int, Value *>> current_def;
        
        void write_var(std::string var_name, int block_id, Value *value) {
            current_def[var_name][block_id] = value;
        }

        void write_var(std::string var_name, Value *value) {
            current_def[var_name][current_block->id] = value;
        }

        Value *read_var(std::string var_name, BasicBlock *block) {
            // Local value numbering
            auto m = current_def[var_name];
            if (m.find(block->id) != m.end())
                return current_def[var_name][block->id];
            
            // Global value numbering
            return read_var_recursive(var_name, block);
        }

        Value *read_var_recursive(std::string var_name, BasicBlock *block) {
            Value *value;
            std::cout << "Blocks: " << block->preds.size() << std::endl;
            if (block->preds.size() == 1) {
                // One predessor, no phi needed
                value = read_var(var_name, block->preds[0]);
            } else {
                assert(false);
            }
            write_var(var_name, block->id, value);
            return value;
        }

        int var_id_counter;
        int next_var_id() {
            return ++var_id_counter;
        }

        int block_id_counter;
        int next_block_id() {
            return ++block_id_counter;
        }

        BasicBlock *new_basic_block() {
            BasicBlock *new_block = new BasicBlock(next_block_id());
            current_ir_function->blocks.push_back(new_block);
            return new_block;
        }

    public:
        explicit Irgen(std::vector<Function *> functions) 
            : var_id_counter(0), block_id_counter(0), functions(functions) {}

        explicit Irgen(std::string source) : Irgen(Parser(source).parse()) {}

        Value *gen(Expression *exp) {
            switch(exp->type) {
                case Expression::IDENT: {
                    return read_var(exp->ident, current_block);
                }
                case Expression::LITERAL: {
                    Const *con = new Const(next_var_id(), 
                        new PrimitiveType(exp->literal.type), 
                        exp->literal.value);
                    current_block->append_instruction(con);
                    return static_cast<Value *>(con);
                }
                case Expression::BINARY: {
                    Value *lhs = gen(exp->binary.lhs);
                    Value *rhs = gen(exp->binary.rhs);
                    BinOp *op = new BinOp(next_var_id(), lhs, rhs, exp->binary.type);
                    current_block->append_instruction(op);
                    return static_cast<Value *>(op);
                }
                case Expression::UNARY: {
                    Value *value = gen(exp->unary.value);
                    UnaryOp *op = new UnaryOp(next_var_id(), value, exp->unary.type);
                    current_block->append_instruction(op);
                    return static_cast<Value *>(op);
                }
                case Expression::CALL: {
                    // Generate argument expressions
                    std::vector<Value *> arg_values;
                    for (Expression *arg : exp->call.args) arg_values.push_back(gen(arg));
                    
                    Call *call_exp = new Call(next_var_id(), exp->call.function_name, arg_values);
                    current_block->append_instruction(call_exp);
                    return static_cast<Value *>(call_exp);
                }
            }
        }

        // TODO: rename these, no point calling them all gen
        Instruction *gen(Statement *smt) {
            switch (smt->type) {
                case Statement::RETURN: {
                    Value *val = gen(smt->ret);
                    Instruction *ins = new Ret(val);
                    current_block->append_instruction(ins);
                    return ins;
                }
                case Statement::BLOCK: {
                    BasicBlock *new_block = gen(smt->block);
                    BasicBlock *continue_block = new_basic_block();
                    
                    new_block->preds.push_back(current_block);
                    continue_block->preds.push_back(current_block);
                    
                    current_block->append_instruction(new Branch(new_block->id));
                    new_block->append_instruction(new Branch(continue_block->id));
                    current_block = continue_block;
                    return NULL; 
                }
                case Statement::IF: {
                    // TODO: else/elseif statements

                    Value *condition = gen(smt->ifs.condition);

                    BasicBlock *if_block = gen(smt->ifs.body->block);
                    BasicBlock *continue_block = new_basic_block();
                    
                    // current block to if block or continue block
                    current_block->append_instruction(
                        new ConditionalBranch(if_block->id, continue_block->id, condition));
                        
                        // if block to continue block
                    if(!if_block->is_terminated())
                        if_block->append_instruction(new Branch(continue_block->id));
                        
                    // Continue from continue block
                    current_block = continue_block;
                    return NULL;
                }
                case Statement::ASSIGN: {
                    std::string var_name = smt->assign.variable->ident;
                    
                    Value *value = gen(smt->assign.value);
                    write_var(var_name, value);
                    
                    return NULL;
                }
                case Statement::FOR: {
                    assert(false);
                }
            }
        }

        BasicBlock *gen(std::vector<Statement *> block) {
            BasicBlock *old_block = current_block;
            BasicBlock *new_block = new_basic_block();
            current_block = new_block;
            for (Statement *s : block) { gen(s); }
            current_block = old_block;
            return new_block;
        }

        IrFunction *gen(Function *func) {
            current_source_function = func;
            
            // Create ir function
            IrFunction *ir_func = new IrFunction(func->name, func->arguments, func->returns);
            current_ir_function = ir_func;
            
            // Create entry block
            BasicBlock *entry_block = new_basic_block();
            current_block = entry_block;

            // Generate argument values
            for (auto arg : current_source_function->arguments) {
                Type *type = std::get<0>(arg);
                std::string name = std::get<1>(arg);
                Arg *arg_value = new Arg(next_var_id(), type, name);
                current_block->append_instruction(arg_value);
                write_var(name, arg_value);
            }

            // Generate entry block
            for (Statement *s : func->body->block) gen(s);
            
            return ir_func;
        }

        std::vector<IrFunction *> gen(std::vector<Function *> funcs) {
            std::vector<IrFunction *> ir_funcs = {};
            for (Function *func : funcs) {
                ir_funcs.push_back(gen(func));
            }
            return ir_funcs;
        }

        std::string to_string() {
            std::ostringstream stream;
            std::vector<IrFunction *> ir_funcs = gen(functions);
            for(IrFunction *func : ir_funcs) {
                stream << *func <<std::endl;
            }

            return stream.str();
        }
};

