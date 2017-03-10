#include "includes/irgen.h"

Irgen *NewIrgen() {
	Irgen *irgen = malloc(sizeof(Irgen));
    irgen->module = LLVMModuleCreateWithName("module");
}

// proc main :: -> int {
//     return 100
// }

LLVMTypeRef CompileType(Exp *e) {
    switch(e->type) {
        case identExp:
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

LLVMValueRef CompileFunction(Irgen *irgen, Dcl *d) {
    ASSERT(d->type == functionDcl, "Expected function declaration");
    
    // compile argument types
    int argCount = d->node.function.argCount;
    LLVMTypeRef *argTypes = malloc(argCount * sizeof(Dcl));
    for (int i = 0; i < argCount; i++) {
        argTypes[i] = CompileType(d->node.function.args[i].node.argument.type);
    }

    // compile return type
    LLVMTypeRef returnType = CompileType(d->node.function.returnType);

    // make function type
    LLVMTypeRef functionType = LLVMFunctionType(returnType, argTypes, argCount, 0); 

    // add function to module
    irgen->function = LLVMAddFunction(
        irgen->module, 
        d->node.function.name->node.ident.name,
        returnType);

    // create entry block and builder
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
    irgen->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(irgen->builder, entry);

    // allocate arguments in entry block
    for (int i = 0; i < argCount; i++) {
        // allocate space for argument
        LLVMValueRef arg = LLVMBuildAlloca(
            irgen->builder, 
            argTypes[i], 
            d->node.function.args[i].node.argument.name->node.ident.name);

        // store argument in allocated space
        LLVMBuildStore(
            irgen->builder,
            LLVMGetParam(irgen->function, i),
            arg);
    }

    return irgen->function;
}

void CompileBlock(Irgen *irgen, Smt *s) {
    ASSERT(s->type == blockSmt, "Expected block statment");
    
    // Compile all statements in block
    for (int i = 0; i < s->node.block.count; i++) {
        CompileSmt(irgen, &s->node.block.smts[i]);
    }
}

void CompileReturn(Irgen *irgen, Smt *s) {
    ASSERT(s->type == returnSmt, "Expected a return statement");

    LLVMTypeRef returnType = LLVMGetReturnType(LLVMTypeOf(irgen->function));

    // build return instruction
    LLVMBuildRet(
        irgen->builder, 
        Cast(irgen, CompileExp(irgen, s->node.ret.result), returnType));
}

void CompileSmt(Irgen *irgen, Smt *s) {
    switch (s->type) {
        case blockSmt:
            CompileBlock(irgen, s);
            break;
        case returnSmt:
            CompileReturn(irgen, s);
            break;
        default:
            ASSERT(false, "TODO");
    }
}

LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type) {
    LLVMTypeRef valueType = LLVMTypeOf(value);

    // create name base on value name + "_cast"
    char *valueName = LLVMGetValueName(value);
    char castName[(strlen(valueName) + 5) * sizeof(char)];
    strcpy(castName, valueName);
    strcpy(castName, "_cast");

    switch (LLVMGetTypeKind(valueType)) {
        // float type    
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
            switch(LLVMGetTypeKind(type)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFPCast(irgen->builder, value, type, castName);
                case LLVMIntegerTypeKind:
                    return LLVMBuildFPToSI(irgen->builder, value, type, castName);
                default:
                    ASSERT(false, "Casting float to non float/int type");
            }

        // integer type
        case LLVMIntegerTypeKind:
            switch(LLVMGetTypeKind(type)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildSIToFP(irgen->builder, value, type, castName);
                case LLVMIntegerTypeKind:
                    return LLVMBuildIntCast(irgen->builder, value, type, castName);
                default:
                    ASSERT(false, "Casting integer to non float/int type");
            }

        default:
            ASSERT(false, "Cannot cast unknown LLVM type");
    }
}

LLVMValueRef CompileLiteral(Irgen *irgen, Exp *e) {
    ASSERT(e->type == literalExp, "Expected literal expression");
    
    switch (e->node.literal.type) {
        case INT:
            return LLVMConstIntOfString(LLVMInt64Type(), e->node.literal.value, 10);
        case FLOAT:
            return LLVMConstRealOfString(LLVMFloatType(), e->node.literal.value);
        case HEX:
            return LLVMConstIntOfString(LLVMInt64Type(), e->node.literal.value, 16);
        case OCTAL:
            return LLVMConstIntOfString(LLVMInt64Type(), e->node.literal.value, 8);
        case STRING:
            ASSERT(false, "TODO: implement strings");
        default:
            ASSERT(false, "Unexpected literal type");
    }
}

LLVMValueRef CompileExp(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case literalExp:
            CompileLiteral(irgen, e);
        default:
            ASSERT(false, "Unknow expression type");
    }

    return NULL;
}