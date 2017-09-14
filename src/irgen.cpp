void Irgen::write_var(std::string var_name, int block_id, Value *value) {
    current_def[var_name][block_id] = value;
}

void Irgen::write_var(std::string var_name, Value *value) {
    current_def[var_name][_current_block->id] = value;
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
        assert(false);
    }
    write_var(var_name, block->id, value);
    return value;
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

void Irgen::gen_if_branch(const Statement *smt, BasicBlock *parent_block, 
    BasicBlock *end_block) {
    
    assert(smt->type == Statement::IF);
    
    if(!smt->ifs.condition) {
        // Statment is an else block, gen and exit
        BasicBlock *out_block = gen_block(parent_block, smt->ifs.body->block);
        
        // If "else" doesnt terminate, branch to end block
        if (!out_block->is_terminated()) {
            out_block->append_instruction(new Branch(end_block->id));
            _current_block = end_block;
        } else {
            _current_block = out_block;
            destroy_basic_block(end_block->id);
        }
        
        return;
    }

    // Generate conditional
    Value *condition = gen(smt->ifs.condition);
    
    // If we wernt passed a parent block, assume current block
    if (!parent_block)
        parent_block = _current_block;

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
    _current_block = false_block;

    // Generate chaining elseif/else statements
    if (smt->ifs.elses)
        gen_if_branch(smt->ifs.elses, false_block, end_block);
        
    _current_block = end_block;
}

// TODO: rename these, no point calling them all gen
Instruction *Irgen::gen(const Statement *smt) {
    switch (smt->type) {
        case Statement::RETURN: {
            Value *val = gen(smt->ret);
            Instruction *ins = new Ret(val);
            _current_block->append_instruction(ins);
            return ins;
        }
        case Statement::BLOCK: {
            BasicBlock *new_block = new_basic_block();
            BasicBlock *out_block = gen_block(new_block, smt->block);
            BasicBlock *continue_block = new_basic_block();
            
            new_block->preds.push_back(_current_block);
            continue_block->preds.push_back(_current_block);
            
            _current_block->append_instruction(new Branch(new_block->id));
            out_block->append_instruction(new Branch(continue_block->id));
            _current_block = continue_block;
            return NULL; 
        }
        case Statement::IF: {
            BasicBlock *end_block = new_basic_block();
            gen_if_branch(smt, NULL, end_block);
            
            return NULL;
        }
        case Statement::ASSIGN: {
            std::string var_name = static_cast<IdentExpression *>(smt->assign.variable)->ident;
            
            Value *value = gen(smt->assign.value);
            write_var(var_name, value);
            
            return NULL;
        }
        case Statement::FOR: {
            assert(false);
        }
    }
}

BasicBlock *Irgen::gen_block(BasicBlock *target_block, const std::vector<Statement *> block) {
    BasicBlock *old_block = _current_block;
    _current_block = target_block;
    for (Statement *s : block) { gen(s); }
    BasicBlock *out_block = _current_block;
    _current_block = old_block;
    return out_block;
}

IrFunction *Irgen::gen(Function *func) {
    current_source_function = func;
    
    // Create ir function
    IrFunction *ir_func = new IrFunction(func->name, func->arguments, func->returns);
    current_ir_function = ir_func;
    
    // Create entry block
    BasicBlock *entry_block = new_basic_block();
    _current_block = entry_block;

    // Generate argument values
    for (auto arg : current_source_function->arguments) {
        Type *type = std::get<0>(arg);
        std::string name = std::get<1>(arg);
        Arg *arg_value = new Arg(next_var_id(), type, name);
        _current_block->append_instruction(arg_value);
        write_var(name, arg_value);
    }

    // Generate entry block
    for (Statement *s : func->body->block) gen(s);
    
    return ir_func;
}

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