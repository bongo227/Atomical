#pragma once

#include "all.h"

#include <llvm-c/Core.h>

struct _Irgen {
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMValueRef function;
    LLVMBasicBlockRef block;
};

typedef struct _Irgen Irgen;

Irgen *NewIrgen();

LLVMValueRef CompileExp(Irgen *irgen, Exp *e);
LLVMTypeRef CompileType(Exp *e);
LLVMValueRef CompileLiteralExp(Irgen *irgen, Exp *e);

void CompileDcl(Irgen *irgen, Dcl *d);
LLVMValueRef CompileFunction(Irgen *i, Dcl *d);

void CompileSmt(Irgen *i, Smt *s);
void CompileBlock(Irgen *i, Smt *s);

LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type);