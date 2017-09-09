class Irgen {
    private:
        std::vector<Function *> functions;
        BasicBlock *current_block;
        Function *current_function;

        std::map<std::string, std::map<int, Value *>> current_def;
        
        void write_var(std::string var_name, int block_id, Value *value) {
            current_def[var_name][block_id] = value;
        }

        void write_var(std::string var_name, Value *value) {
            current_def[var_name][current_block->id] = value;
        }

        Value *read_var(std::string var_name, int block_id) {
            auto m = current_def[var_name];
            if (m.find(block_id) != m.end())
                return current_def[var_name][block_id];
            assert(false); // global value numbering
        }

        Value *read_var(std::string var_name) {
            return read_var(var_name, current_block->id);
        }

        int var_id;
        int next_var_id() {
            return ++var_id;
        }

        int block_id;
        int next_block_id() {
            return ++block_id;
        }

    public:
        explicit Irgen(std::vector<Function *> functions) : 
            var_id(0), block_id(0), functions(functions) {}

        explicit Irgen(std::string source) : Irgen(Parser(source).parse()) {}

        Value *gen(Expression *exp) {
            switch(exp->type) {
                case Expression::IDENT: {
                    return read_var(exp->ident);
                }
                case Expression::LITERAL: {
                    return new Const(
                        new PrimitiveType(exp->literal.type), 
                        exp->literal.value);
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

        Instruction *gen(Statement *smt) {
            switch (smt->type) {
                case Statement::RETURN: {
                    Value *val = gen(smt->ret);
                    Instruction *ins = new Ret(val);
                    current_block->append_instruction(ins);
                    return ins;
                }
                default: {
                    assert(false);
                }
            }
        }

        BasicBlock gen(std::vector<Statement *> block) {
            BasicBlock *new_block = new BasicBlock(next_block_id());
            current_block = new_block;
            for (Statement *s : block) { gen(s); }
            return *new_block;
        }

        IrFunction gen(Function *func) {
            current_function = func;
            
            // Create entry block
            BasicBlock *entry_block = new BasicBlock(next_block_id());
            current_block = entry_block;

            // Generate argument values
            for (auto arg : current_function->arguments) {
                Type *type = std::get<0>(arg);
                std::string name = std::get<1>(arg);
                Arg *arg_value = new Arg(next_var_id(), type, name);
                current_block->append_instruction(arg_value);
                write_var(name, arg_value);
            }

            // Generate entry block
            for (Statement *s : func->body->block) gen(s);

            // Create ir function
            IrFunction ir_func = IrFunction(func->name, func->arguments, func->returns);
            ir_func.append_block(*entry_block);
            
            return ir_func;
        }

        std::vector<IrFunction> gen(std::vector<Function *> funcs) {
            std::vector<IrFunction> ir_funcs = {};
            for (Function *func : funcs) {
                ir_funcs.push_back(gen(func));
            }
            return ir_funcs;
        }

        std::string to_string() {
            std::ostringstream stream;
            std::vector<IrFunction> ir_funcs = gen(functions);
            for(IrFunction func : ir_funcs) {
                stream << func <<std::endl;
            }

            return stream.str();
        }
};

