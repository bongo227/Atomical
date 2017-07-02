#include "includes/ssa.h"

Ssa *new_ssa() {
    Ssa *ssa = malloc(sizeof(Ssa));
    memset(ssa, 0, sizeof(Ssa));
    return ssa;
}

BasicType ssa_type(Exp *exp) {
    ASSERT(exp->type == identExp, "SSA only handles identExp types");
    if (strcmp(exp->ident.name, "int") == 0) return i64;
    if (strcmp(exp->ident.name, "i64") == 0) return i64;
    if (strcmp(exp->ident.name, "i32") == 0) return i32;
    if (strcmp(exp->ident.name, "i16") == 0) return i16;
    if (strcmp(exp->ident.name, "i8") == 0) return i8;

    if (strcmp(exp->ident.name, "float") == 0) return f32;
    if (strcmp(exp->ident.name, "f32") == 0) return f32;
    if (strcmp(exp->ident.name, "f64") == 0) return f64;

    ASSERT(false, "Need more advance types for SSA");
    return NONE;
}

Instruction *ssa_instruction_arg(
    Ssa *ssa, 
    BasicBlock *block, 
    BasicType type, 
    char *ident) {

    Variable *v = malloc(sizeof(Variable));
    v->id = ssa->var_counter++;
    v->type = type;

    Instruction *i = malloc(sizeof(Instruction));
    i->out = v;
    i->instruction_arg.ident = ident;

    // Insert instruction into block
    if(block->last != NULL) block->last->next = i;
    block->last = i;
    if(block->first == NULL) block->first = i;

    ssa->current_def[v->id][block->id] = i;

    return i;
}

Procedure *ssa_function(Ssa *ssa, Dcl *dcl) {
    Procedure *proc = calloc(1, sizeof(Procedure));
    BasicBlock *entry = calloc(1, sizeof(BasicBlock));
    proc->entryBlock = entry;

    for (int i = 0; i < dcl->function.argCount; i++) {
        ssa_instruction_arg(
            ssa, 
            entry, 
            ssa_type(dcl->function.args[i].argument.type), 
            dcl->function.args[i].argument.name);
    }

    return proc;
}
