#include "includes/irgen.h"

#include <llvm-c\Core.h>

Irgen *NewIrgen() {
	Irgen *i = malloc(sizeof(Irgen));
    i->module = LLVMModuleCreateWithName("module");
}

// proc main :: -> int {
//     return 100
// }

LLVMTypeRef CompileType(Exp *e) {
    switch(e->type) {
        case IDENT:
            if (strcmp(e->node.ident.name, "int") == 0) return LLVMInt64Type();
            if (strcmp(e->node.ident.name, "i64") == 0) return LLVMInt64Type();
            if (strcmp(e->node.ident.name, "i32") == 0) return LLVMInt32Type();
            if (strcmp(e->node.ident.name, "i16") == 0) return LLVMInt16Type();
            if (strcmp(e->node.ident.name, "i8") == 0) return LLVMInt8Type();

            if (strcmp(e->node.ident.name, "float") == 0) return LLVMFloatType();
            if (strcmp(e->node.ident.name, "f32") == 0) return LLVMFloatType();
            if (strcmp(e->node.ident.name, "f64") == 0) return LLVMDoubleType();
        default:
            ASSERT(false, "Expected a type");
    }
}

LLVMValueRef CompileFunction(Irgen *i, Dcl *d) {
    // compile argument types
    int argCount = d->node.function.argCount
    LLVMTypeRef *argTypes = malloc(argCount * sizeof(Dcl));
    for (int i = 0; i < argCount; i++) {
        argTypes++ = CompileType(d->node.function.args++);
    }

    // compile return type
    LLVMTypeRef returnType = CompileType(d->node.function.returnType);

    // make function type
    LLVMTypeRef functionType = LLVMFunctionType(returnType, argTypes, argCount, 0); 

    // add function to module
    LLVMValueRef function = LLVMAddFunction(
        i->module, 
        d->node.function.name->node.ident.name
        returnType);

    // create entry block and builder
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(function, "entry");
    i->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(i->builder, entry);

    // allocate arguments in entry block
    for (int i = 0; i < argCount; i++) {
        // allocate space for argument
        LLVMValueRef arg = LLVMBuildAlloca(
            i->builder, 
            argTypes[i], 
            d->node.function.args[i]->node.argument.name);

        // store argument in allocated space
        LLVMBuildStore(
            i->builder,
            LLVMGetParam(function, i),
            arg);
    }

    return function;
}

void CompileBlock(Irgen *i, Smt *s) {
    ASSERT(s->type == blockSmt, "Expected block statment");
    
}

void CompileSmt(Irgen *i, Smt *s) {
    switch (s->type) {
        case blockSmt:
            CompileBlock(i, s);
            break;
    }
}



LLVMValueRef CompileExp(Exp *e) {
    // switch(e->type) {
    //     case LITERAL:

    // }

    return NULL
}