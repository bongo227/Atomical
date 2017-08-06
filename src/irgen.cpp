class Irgen {
    public:
        std::vector<Function *> functions;
        std::vector<Instruction *> currentBlock;

    public:
        explicit Irgen(std::vector<Function *> functions) : functions(functions) {}
        explicit Irgen(std::string source) : Irgen(Parser(source).parse()) {}

        Value *gen(Expression *exp) {
            switch(exp->type) {
                case Expression::LITERAL: {
                    return new Const(new PrimitiveType(exp->literal.type), exp->literal.value);
                }
                case Expression::BINARY: {
                    Value *lhs = gen(exp->binary.lhs);
                    Value *rhs = gen(exp->binary.rhs);
                    BinOp *op = new BinOp(lhs, rhs, exp->binary.type);
                    currentBlock.push_back(op);
                    return static_cast<Value *>(op);
                }
                case Expression::UNARY: {
                    Value *value = gen(exp->unary.value);
                    UnaryOp *op = new UnaryOp(value, exp->unary.type);
                    currentBlock.push_back(op);
                    return static_cast<Value *>(op);
                }
            }
        }

        Instruction *gen(Statement *smt) {
            switch (smt->type) {
                case Statement::RETURN: {
                    Value *val = gen(smt->ret);
                    Instruction *ins = new Ret(val);
                    currentBlock.push_back(ins);
                    return ins;
                }
            }
        }

};

