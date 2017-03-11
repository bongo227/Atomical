#pragma once

#include "all.h"

#include <llvm-c/Core.h>

struct _Irgen {
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMValueRef function;
};

typedef struct _Irgen Irgen;

LLVMTypeRef CompileType(Exp *e);
LLVMValueRef CompileFunction(Irgen *i, Dcl *d);
void CompileBlock(Irgen *i, Smt *s);
void CompileSmt(Irgen *i, Smt *s);
void CompileDcl(Irgen *irgen, Dcl *d);
LLVMValueRef CompileExp(Irgen *irgen, Exp *e);
LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type);