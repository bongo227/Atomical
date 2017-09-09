class Irgen {
    private:
        std::vector<Function *> functions;
        BasicBlock *current_block;
        Function *current_function;

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
                    // TODO: replace with local identifier list
                    for (auto arg : current_function->arguments) {
                        Type *type = std::get<0>(arg);
                        std::string name = std::get<1>(arg);

                        if (name == exp->ident) {
                            return new Arg(type, name);
                        }
                    }

                    assert(false);
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
            current_block = new BasicBlock(next_block_id());
            for (Statement *s : block) { gen(s); }
            return *current_block;
        }

        IrFunction gen(Function *func) {
            IrFunction ir_func = IrFunction(func->name, func->arguments, func->returns);
            ir_func.append_block(gen(func->body->block));
            return ir_func;
        }

        std::vector<IrFunction> gen(std::vector<Function *> funcs) {
            std::vector<IrFunction> ir_funcs = {};
            for (Function *func : funcs) {
                current_function = func;
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

