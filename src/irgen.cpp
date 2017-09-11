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

        void destroy_basic_block(int block_id) {
            int i = 0;
            for(BasicBlock *block : current_ir_function->blocks) {
                if (block->id == block_id) break; 
                i++;
            }

            current_ir_function->blocks.erase(current_ir_function->blocks.begin() + i);
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

        void gen_if_branch(Statement *smt, BasicBlock *parent_block, 
            BasicBlock *end_block) {
            
            assert(smt->type == Statement::IF);
            
            if(!smt->ifs.condition) {
                // Statment is an else block, gen and exit
                BasicBlock *out_block = gen_block(parent_block, smt->ifs.body->block);
                
                // If "else" doesnt terminate, branch to end block
                if (!out_block->is_terminated()) {
                    out_block->append_instruction(new Branch(end_block->id));
                    current_block = end_block;
                } else {
                    current_block = out_block;
                    destroy_basic_block(end_block->id);
                }
                
                return;
            }

            // Generate conditional
            Value *condition = gen(smt->ifs.condition);
            
            // If we wernt passed a parent block, assume current block
            if (!parent_block)
                parent_block = current_block;

            // true block for if the conditional is true
            BasicBlock *true_block = new_basic_block();

            // false block is either the next else/elseif block if the block to continue from
            BasicBlock *false_block = end_block;
            if(smt->ifs.elses)
                false_block = new_basic_block();

            // generate the true block
            BasicBlock *out_block = gen_block(true_block, smt->ifs.body->block);
            if (!out_block->is_terminated())
                out_block->append_instruction(new Branch(end_block->id));

            // Branch into "if branch" from parent
            parent_block->append_instruction(
                new ConditionalBranch(true_block->id, false_block->id, condition));
                
            // TODO: do we need this?
            current_block = false_block;

            // Generate chaining elseif/else statements
            if (smt->ifs.elses)
                gen_if_branch(smt->ifs.elses, false_block, end_block);
                
            current_block = end_block;
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
                    BasicBlock *new_block = new_basic_block();
                    BasicBlock *out_block = gen_block(new_block, smt->block);
                    BasicBlock *continue_block = new_basic_block();
                    
                    new_block->preds.push_back(current_block);
                    continue_block->preds.push_back(current_block);
                    
                    current_block->append_instruction(new Branch(new_block->id));
                    out_block->append_instruction(new Branch(continue_block->id));
                    current_block = continue_block;
                    return NULL; 
                }
                case Statement::IF: {
                    BasicBlock *end_block = new_basic_block();
                    gen_if_branch(smt, NULL, end_block);
                    
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

        BasicBlock *gen_block(BasicBlock *target_block, std::vector<Statement *> block) {
            BasicBlock *old_block = current_block;
            current_block = target_block;
            for (Statement *s : block) { gen(s); }
            BasicBlock *out_block = current_block;
            current_block = old_block;
            return out_block;
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

