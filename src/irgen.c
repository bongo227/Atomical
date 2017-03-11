#include "includes/irgen.h"

Irgen *NewIrgen() {
	Irgen *irgen = malloc(sizeof(Irgen));
    irgen->module = LLVMModuleCreateWithName("module");
}

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
    LLVMTypeRef *argTypes = malloc(argCount * sizeof(LLVMTypeRef));
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
        functionType);

    // create entry block and builder
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
    irgen->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(irgen->builder, entry);

    // allocate arguments in entry block
    for (int i = 0; i < argCount; i++) {
        // get argument node
        Dcl *argNode = d->node.function.args + i;
        char *argName = argNode->node.argument.name->node.ident.name;

        // allocate space for argument
        LLVMValueRef argAlloc = LLVMBuildAlloca(
            irgen->builder, 
            argTypes[i], 
            argName);
        
        // store alloc in node
        argNode->llvmValue = argAlloc;

        // store argument in allocated space
        LLVMValueRef argValue = LLVMGetParam(irgen->function, i);
        LLVMBuildStore(irgen->builder, argValue, argAlloc);
    }

    CompileBlock(irgen, d->node.function.body);

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

    LLVMTypeRef functionType = LLVMTypeOf(irgen->function);
    LLVMTypeRef returnType = LLVMGetReturnType(LLVMGetReturnType(functionType));

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

    if(LLVMTypeOf(value) == type) return value;

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

LLVMValueRef CompileLiteralExp(Irgen *irgen, Exp *e) {
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

LLVMValueRef CompileBinaryExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == binaryExp, "Expected binary expression");

    LLVMValueRef left = CompileExp(irgen, e->node.binary.left);
    LLVMValueRef right = CompileExp(irgen, e->node.binary.right);

    LLVMTypeRef leftType = LLVMTypeOf(left);
    LLVMTypeRef rightType = LLVMTypeOf(right);
    LLVMTypeRef nodeType;    

    // Check for unequal types
    if (leftType != rightType) {
        LLVMTypeKind leftKind = LLVMGetTypeKind(leftType);
        LLVMTypeKind rightKind = LLVMGetTypeKind(rightType);
        
        if(leftKind == LLVMIntegerTypeKind && rightKind == LLVMIntegerTypeKind) {
            nodeType = LLVMInt64Type();
            Cast(irgen, left, nodeType);
            Cast(irgen, right, nodeType);
        } else {
            // one or more sides are float so premote both sides to float
            nodeType = LLVMDoubleType();
            Cast(irgen, left, nodeType);
            Cast(irgen, right, nodeType);
        }
    } else {
        nodeType = leftType;
    }

    LLVMTypeKind nodeTypeKind = LLVMGetTypeKind(nodeType);

    // build name
    char *leftName = LLVMGetValueName(left);
    char *rightName = LLVMGetValueName(right);
    char name[strlen(leftName) + 1 + strlen(rightName)];
    strcpy(name, leftName);
    strcpy(name, TokenName(e->node.binary.op.type));
    strcpy(name, rightName);

    switch (e->node.binary.op.type) {
        case ADD:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFAdd(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildAdd(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot add non float/int type");
            }
        case SUB:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFSub(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSub(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot sub non float/int type");
            }
        case MUL:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFMul(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildMul(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot multiply non float/int type");
            }
        case QUO:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFDiv(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSDiv(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot divide non float/int type");
            }
        case REM:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFRem(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSRem(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot mod non float/int type");
            }
        case XOR:
        case LSS:
        case LEQ:
        case SHL:
        case GTR:
        case GEQ:
        case SHR:
        case EQL:
        case NEQ:
        case AND_NOT:
        case AND:
        case LAND:
        case OR:
        case LOR:
            ASSERT(false, "TODO");
        default:
            ASSERT(false, "Unknown binary operator");
    }
}

LLVMValueRef CompileIdentExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == identExp, "Expected identifier expression");

    // Get alloc from ident
    Dcl *dcl = (Dcl *)(e->node.ident.obj->node);
    LLVMValueRef alloc = dcl->llvmValue;

    return LLVMBuildLoad(irgen->builder, alloc, e->node.ident.name);
}

LLVMValueRef CompileExp(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case literalExp:
            return CompileLiteralExp(irgen, e);
        case binaryExp:
            return CompileBinaryExp(irgen, e);
        case identExp:
            return CompileIdentExp(irgen, e);
        default:
            ASSERT(false, "Unknow expression type");
    }

    return NULL;
}