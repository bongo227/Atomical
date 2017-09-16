void Irgen::write_var(std::string var_name, int block_id, Value *value) {
    current_def[var_name][block_id] = value;
}

void Irgen::write_var(std::string var_name, Value *value) {
    current_def[var_name][current_block->id] = value;
}

Value *Irgen::read_var(std::string var_name, BasicBlock *block) {
    // Local value numbering
    auto m = current_def[var_name];
    if (m.find(block->id) != m.end())
        return current_def[var_name][block->id];
    
    // Global value numbering
    return read_var_recursive(var_name, block);
}

Value *Irgen::read_var_recursive(std::string var_name, BasicBlock *block) {
    Value *value;
    std::cout << "Blocks: " << block->preds.size() << std::endl;
    if (block->preds.size() == 1) {
        // One predessor, no phi needed
        value = read_var(var_name, block->preds[0]);
    } else {
        Phi *phi = new Phi(next_var_id(), {});
        block->append_instruction(phi); 
        value = phi;
        write_var(var_name, block->id, value);
        add_phi_operands(var_name, block, phi);
    }
    write_var(var_name, block->id, value);
    return value;
}

void Irgen::add_phi_operands(std::string var_name, BasicBlock *block, Phi *phi) {
    for(BasicBlock *pred : block->preds) {
        PhiOperand op = PhiOperand(pred->id, read_var(var_name, pred));
        phi->append_operand(op);
    }
}

BasicBlock *Irgen::new_basic_block() {
    BasicBlock *new_block = new BasicBlock(next_block_id());
    current_ir_function->blocks.push_back(new_block);
    return new_block;
}

void Irgen::destroy_basic_block(int block_id) {
    int i = 0;
    for(BasicBlock *block : current_ir_function->blocks) {
        if (block->id == block_id) break; 
        i++;
    }

    current_ir_function->blocks.erase(current_ir_function->blocks.begin() + i);
}

Irgen::Irgen(std::vector<Function *> functions) : var_id_counter(0), block_id_counter(0), 
    functions(functions) {}

Irgen::Irgen(std::string source) : Irgen(Parser(source).parse()) {}

Value *Irgen::gen(const Expression *exp) {
    return exp->code_gen(this);
}

// TODO: rename these, no point calling them all gen
void Irgen::gen(const Statement *smt) {
    smt->code_gen(this);
}

BasicBlock *Irgen::gen_block(BasicBlock *target_block, const BlockStatement *block) {
    BasicBlock *old_block = current_block;
    current_block = target_block;
    for (Statement *s : block->statements) { gen(s); }
    BasicBlock *out_block = current_block;
    current_block = old_block;
    return out_block;
}

IrFunction *Irgen::gen(Function *func) {
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
    for (Statement *s : func->body->statements) gen(s);
    
    return ir_func;
}

// TODO: move this into BlockStatement
std::vector<IrFunction *> Irgen::gen(std::vector<Function *> funcs) {
    std::vector<IrFunction *> ir_funcs = {};
    for (Function *func : funcs) {
        ir_funcs.push_back(gen(func));
    }
    return ir_funcs;
}

std::string Irgen::to_string() {
    std::ostringstream stream;
    std::vector<IrFunction *> ir_funcs = gen(functions);
    for(IrFunction *func : ir_funcs) {
        stream << *func <<std::endl;
    }

    return stream.str();
}