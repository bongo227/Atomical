#include "includes/irgen.h"

Irgen *NewIrgen() {
	Irgen *irgen = malloc(sizeof(Irgen));
    irgen->module = LLVMModuleCreateWithName("module");

    return irgen;
}

LLVMTypeRef CompileType(Exp *e) {
    switch(e->type) {
        case identExp:
            if (strcmp(e->ident.name, "int") == 0) return LLVMInt64Type();
            if (strcmp(e->ident.name, "i64") == 0) return LLVMInt64Type();
            if (strcmp(e->ident.name, "i32") == 0) return LLVMInt32Type();
            if (strcmp(e->ident.name, "i16") == 0) return LLVMInt16Type();
            if (strcmp(e->ident.name, "i8") == 0) return LLVMInt8Type();

            if (strcmp(e->ident.name, "float") == 0) return LLVMFloatType();
            if (strcmp(e->ident.name, "f32") == 0) return LLVMFloatType();
            if (strcmp(e->ident.name, "f64") == 0) return LLVMDoubleType();
        case arrayTypeExp: {
            LLVMTypeRef elementType = CompileType(e->arrayType.type);
            int length = atoi(e->arrayType.length->literal.value);
            return LLVMArrayType(elementType, length);
        }
        default:
            ASSERT(false, "Expected a type");
    }
}

LLVMValueRef CompileFunction(Irgen *irgen, Dcl *d) {
    ASSERT(d->type == functionDcl, "Expected function declaration");
    
    // compile argument types
    int argCount = d->function.argCount;
    LLVMTypeRef *argTypes = malloc(argCount * sizeof(LLVMTypeRef));
    for (int i = 0; i < argCount; i++) {
        argTypes[i] = CompileType(d->function.args[i].argument.type);
    }

    // compile return type
    LLVMTypeRef returnType = CompileType(d->function.returnType);

    // make function type
    LLVMTypeRef functionType = LLVMFunctionType(returnType, argTypes, argCount, 0); 

    // add function to module
    irgen->function = LLVMAddFunction(
        irgen->module, 
        d->function.name,
        functionType);

    // add function to node
    d->llvmValue = irgen->function;

    // create entry block and builder
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
    irgen->block = entry;
    irgen->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(irgen->builder, entry);

    // allocate arguments in entry block
    for (int i = 0; i < argCount; i++) {
        // get argument node
        Dcl *argNode = d->function.args + i;
        char *argName = argNode->argument.name;

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

    CompileBlock(irgen, d->function.body);

    // remove last block if empty
    if (LLVMGetFirstInstruction(irgen->block) == NULL) {
        LLVMDeleteBasicBlock(irgen->block);
    }

    irgen->block = NULL;
    return irgen->function;
}

// Sets the current block to the given basic block.
void SetBlock(Irgen *irgen, LLVMBasicBlockRef block) {
    irgen->block = block;
    LLVMPositionBuilderAtEnd(irgen->builder, block);
}

// Compiles a block in the current basic block.
void CompileBlock(Irgen *irgen, Smt *s) {
    ASSERT(s->type == blockSmt, "Expected block statment");
    
    // Compile all statements in block
    for (int i = 0; i < s->block.count; i++) {
        CompileSmt(irgen, &s->block.smts[i]);
    }
}

// Compiles a block at the given basic block.
LLVMBasicBlockRef CompileBlockAt(Irgen *irgen, Smt *s, LLVMBasicBlockRef block) {
    assert(s->type == blockSmt);
    
    // Move to target block
    LLVMBasicBlockRef parent = irgen->block;
    SetBlock(irgen, block);
    
    CompileBlock(irgen, s);
    
    // Restore parent block
    LLVMBasicBlockRef outBlock = irgen->block;
    SetBlock(irgen, parent);

    return outBlock;
}

void CompileReturn(Irgen *irgen, Smt *s) {
    ASSERT(s->type == returnSmt, "Expected a return statement");

    LLVMTypeRef functionType = LLVMTypeOf(irgen->function);
    LLVMTypeRef returnType = LLVMGetReturnType(LLVMGetReturnType(functionType));

    // build return instruction
    LLVMBuildRet(
        irgen->builder, 
        Cast(irgen, CompileExp(irgen, s->ret.result), returnType));
}

// Gets the allocation for an expression
LLVMValueRef GetAlloc(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case identExp: {
            ASSERT(e->ident.obj != NULL, "Identifier doesnt have object");
            
            Dcl *dcl = e->ident.obj->node;
            return dcl->llvmValue;
        }
        case indexExp: {
            // Get the allocation of the expression
            LLVMValueRef alloc = GetAlloc(irgen, e->index.exp);
            
            // Get the element at the index
            LLVMValueRef index = CompileExp(irgen, e->index.index);
            LLVMValueRef zero = LLVMConstInt(LLVMInt64Type(), 0, false);
            LLVMValueRef indices[] = { zero, index };
            
            return LLVMBuildGEP(irgen->builder, alloc, indices, 2, "tmp");
        }
        default:
            ASSERT(false, "Cannot get alloc on unknown expression");
    }
}

void CompileAssignment(Irgen *irgen, Smt *s) {
    ASSERT(s->type == assignmentSmt, "Expected an assignment statement");

    LLVMValueRef alloc = GetAlloc(irgen, s->assignment.left);
    LLVMValueRef exp = CompileExp(irgen, s->assignment.right);
    LLVMBuildStore(irgen->builder, exp, alloc);
}

void CompileIfBranch(Irgen *irgen, Smt *s, LLVMBasicBlockRef block, LLVMBasicBlockRef endBlock) {
    ASSERT(s->type == ifSmt, "Expected if statement");
    
    Exp *cond = s->ifs.cond;
    if (cond == NULL) {
        assert(block != NULL);
        
        // Compile else block and exit
        LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->ifs.body, block);
        SetBlock(irgen, outBlock);
        if (LLVMGetBasicBlockTerminator(outBlock) == NULL) {
            // block is not terminated so continue execution from end block
            LLVMBuildBr(irgen->builder, endBlock);
            SetBlock(irgen, endBlock);
        }

        return;
    }

    // parent block is the block to branch from
    LLVMBasicBlockRef parent;
    if (block == NULL) {
        parent = irgen->block;
    } else {
        parent = block;
    }
    // block if condition is true
    block = LLVMAppendBasicBlock(irgen->function, "if");
    
    // falseBlock is either the next else/elseif block or block to conitue execution
    LLVMBasicBlockRef falseBlock;
    if (s->ifs.elses != NULL) {
        falseBlock = LLVMAppendBasicBlock(irgen->function, "else");
    } else {
        falseBlock = endBlock;
    }

    // compile if block
    LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->ifs.body, block);
    if (LLVMGetBasicBlockTerminator(outBlock) == NULL) {
        // block is not terminated so continue execution from end block
        SetBlock(irgen, outBlock);
        LLVMBuildBr(irgen->builder, endBlock);
        SetBlock(irgen, parent);
    }
    
    // Add the conditional branch
    LLVMValueRef condition = CompileExp(irgen, cond);
    LLVMBuildCondBr(irgen->builder, condition, block, falseBlock);

    // if their is a chaining elseif/else set its parent to the falseBlock
    SetBlock(irgen, falseBlock);
    if(s->ifs.elses != NULL) {
        CompileIfBranch(irgen, s->ifs.elses, falseBlock, endBlock);
    }

    // continue execution from the endBlock
    SetBlock(irgen, endBlock);
}

void CompileIf(Irgen *irgen, Smt *s) {
    LLVMBasicBlockRef endBlock = LLVMAppendBasicBlock(irgen->function, "endBlock");
    CompileIfBranch(irgen, s, NULL, endBlock);
}

void CompileFor(Irgen *irgen, Smt *s) {
    ASSERT(s->type == forSmt, "Expected for statement");

    // Compile loop varible
    CompileDcl(irgen, s->fors.index);

    // compile for body
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(irgen->function, "for");
    LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->fors.body, block);

    // branch into for loop
    LLVMValueRef outerCond = CompileExp(irgen, s->fors.cond);
    LLVMBasicBlockRef continueBlock = LLVMAppendBasicBlock(irgen->function, "endfor");
    LLVMBuildCondBr(irgen->builder, outerCond, block, continueBlock);

    // branch to loop or exit
    SetBlock(irgen, outBlock);
    CompileSmt(irgen, s->fors.inc);
    LLVMValueRef innerCond = CompileExp(irgen, s->fors.cond);
    LLVMBuildCondBr(irgen->builder, innerCond, block, continueBlock);

    // continue from continueBlock
    SetBlock(irgen, continueBlock);
}

void CompileSmt(Irgen *irgen, Smt *s) {
    switch (s->type) {
        case blockSmt:
            CompileBlock(irgen, s);
            break;
        
        case returnSmt:
            CompileReturn(irgen, s);
            break;
        
        case assignmentSmt:
            CompileAssignment(irgen, s);
            break;
        
        case declareSmt:
            CompileDcl(irgen, s->declare);
            break;
        
        case ifSmt:
            CompileIf(irgen, s);
            break;

        case forSmt:
            CompileFor(irgen, s);
            break;
        
        default:
            ASSERT(false, "TODO");
    }
}



void CompileVarible(Irgen *irgen, Dcl *d) {
    // get argument node
    char *varName = d->varible.name;

    // compile expression
    LLVMValueRef exp = CompileExp(irgen, d->varible.value);

    // get the type of the varible declaration
    LLVMTypeRef varType;
    if (d->varible.type != NULL) {
        varType = CompileType(d->varible.type);
        exp = Cast(irgen, exp, varType);
    } else {
        varType = LLVMTypeOf(exp);
    }

    LLVMValueRef varAlloc;
    if (d->varible.value->type == arrayExp) {
        varAlloc = exp;
    } else {
        // allocate space for varible
        varAlloc = LLVMBuildAlloca(
            irgen->builder, 
            varType, 
            varName);
            
        // store argument in allocated space
        LLVMBuildStore(irgen->builder, exp, varAlloc);
    }
        
    // store alloc in node
    d->llvmValue = varAlloc;

}

void CompileDcl(Irgen *irgen, Dcl *d) {
    switch(d->type) {
        case functionDcl:
            CompileFunction(irgen, d);
        case argumentDcl:
            ASSERT(false, "Cannot compile argument outside function declaration");
        case varibleDcl:
            CompileVarible(irgen, d);
    }
}

LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type) {
    LLVMTypeRef valueType = LLVMTypeOf(value);

    if(LLVMTypeOf(value) == type) return value;

    // create name base on value name + "_cast"
    char *valueName = (char *)LLVMGetValueName(value);
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
    
    switch (e->literal.type) {
        case INT:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 10);
        case FLOAT:
            return LLVMConstRealOfString(LLVMFloatType(), e->literal.value);
        case HEX:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 16);
        case OCTAL:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 8);
        case STRING:
            ASSERT(false, "Strings not implemented yet");
        default:
            ASSERT(false, "Unexpected literal type");
    }
}

LLVMValueRef CompileBinaryExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == binaryExp, "Expected binary expression");

    LLVMValueRef left = CompileExp(irgen, e->binary.left);
    LLVMValueRef right = CompileExp(irgen, e->binary.right);

    LLVMTypeRef leftType = LLVMTypeOf(left);
    LLVMTypeRef rightType = LLVMTypeOf(right);
    LLVMTypeRef nodeType;    

    // Check for unequal types
    if (leftType != rightType) {
        LLVMTypeKind leftKind = LLVMGetTypeKind(leftType);
        LLVMTypeKind rightKind = LLVMGetTypeKind(rightType);
        
        if(leftKind == LLVMIntegerTypeKind && rightKind == LLVMIntegerTypeKind) {
            nodeType = LLVMInt64Type();
            left = Cast(irgen, left, nodeType);
            right = Cast(irgen, right, nodeType);
        } else {
            // one or more sides are float so premote both sides to float
            nodeType = LLVMDoubleType();
            left = Cast(irgen, left, nodeType);
            right = Cast(irgen, right, nodeType);
        }
    } else {
        nodeType = leftType;
    }

    LLVMTypeKind nodeTypeKind = LLVMGetTypeKind(nodeType);

    // build name
    char *leftName = (char *)LLVMGetValueName(left);
    char *rightName = (char *)LLVMGetValueName(right);
    char name[strlen(leftName) + 1 + strlen(rightName)];
    strcpy(name, leftName);
    strcpy(name, TokenName(e->binary.op.type));
    strcpy(name, rightName);

    switch (e->binary.op.type) {
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
        case LSS:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealULT, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSLT, left, right, name);
                default:
                    ASSERT(false, "Cannot less than non float/int type");
            }

        case LEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealULE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSLE, left, right, name);
                default:
                    ASSERT(false, "Cannot less than or equal non float/int type");
            }

        case GTR:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUGT, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSGT, left, right, name);
                default:
                    ASSERT(false, "Cannot greater than non float/int type");
            }
        case GEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUGE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSGE, left, right, name);
                default:
                    ASSERT(false, "Cannot greater than or equal non float/int type");
            }

        case EQL:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUEQ, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntEQ, left, right, name);
                default:
                    ASSERT(false, "Cannot equal non float/int type");
            }

        case NEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUNE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntNE, left, right, name);
                default:
                    ASSERT(false, "Cannot not equal non float/int type");
            }

        case LAND:
        case LOR:
        case XOR:
        case SHL:
        case SHR:
        case AND_NOT:
        case AND:
        case OR:
            ASSERT(false, "TODO");
            break;
        default:
            ASSERT(false, "Unknown binary operator");
            break;
    }
}

LLVMValueRef CompileIdentExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == identExp, "Expected identifier expression");

    char *ident = e->ident.name;
    if(strcmp(ident, "true") == 0) return LLVMConstInt(LLVMInt1Type(), 1, false);
    if(strcmp(ident, "false") == 0) return LLVMConstInt(LLVMInt1Type(), 0, false);

    LLVMValueRef alloc = GetAlloc(irgen, e);
    return LLVMBuildLoad(irgen->builder, alloc, e->ident.name);
}

LLVMValueRef CompileUnaryExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == unaryExp, "Expected unary expression");

    LLVMValueRef exp = CompileExp(irgen, e->unary.right);
    switch(e->unary.op.type) {
        case ADD:
            return exp;
        case SUB: {
            // build name
            char *expName = (char *)LLVMGetValueName(exp);
            char *name = alloca(sizeof(char) * (strlen(expName) + 1));
            strcpy(name, "-");
            strcpy(name, expName);

            // insert "-1 * exp" instruction 
            LLVMTypeRef expType = LLVMTypeOf(exp);
            switch(LLVMGetTypeKind(expType)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind: {
                    LLVMValueRef minusOne = LLVMConstReal(expType, -1);
                    return LLVMBuildFMul(irgen->builder, minusOne, exp, name);
                }
                case LLVMIntegerTypeKind: {
                    LLVMValueRef minusOne = LLVMConstInt(expType, -1, true);
                    return LLVMBuildMul(irgen->builder, minusOne, exp, name);
                }
                default:
                    ASSERT(false, "Cannot multiply non float/int type by -1");
            } 
        }
        default:
            ASSERT(false, "Unkown unary operator");
    }
}

LLVMValueRef CompileCallExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == callExp, "Expected call expression");
    
    LLVMValueRef function = GetAlloc(irgen, e->call.function);

    // compile arguments
    int argCount = e->call.argCount;
    LLVMValueRef *args = malloc(argCount * sizeof(LLVMValueRef));
    for(int i = 0; i < argCount; i++) {
        args[i] = CompileExp(irgen, e->call.args + i);
    }

    return LLVMBuildCall(irgen->builder, function, args, argCount, "tmp");
} 

LLVMValueRef CompileArrayExp(Irgen *irgen, Exp *e) {
    assert(e->type == arrayExp);

    int valueCount = e->array.valueCount;
    bool isFloat = false;
    LLVMValueRef *values = alloca(valueCount * sizeof(LLVMValueRef));
    for (int i = 0; i < valueCount; i++) {
        values[i] = CompileExp(irgen, e->array.values + i);
        if (LLVMGetTypeKind(LLVMTypeOf(values[i])) == LLVMFloatTypeKind) {
            isFloat = true;
        }
    }

    LLVMTypeRef castType = isFloat ? LLVMFloatType() : LLVMInt64Type();
    LLVMTypeRef arrayType = LLVMArrayType(castType, valueCount);

    LLVMValueRef arrayAlloc = LLVMBuildAlloca(
        irgen->builder, 
        arrayType,
        "tmp");

    for (int i = 0; i < valueCount; i++) {
        values[i] = Cast(irgen, values[i], castType);
        
        LLVMValueRef indices[2] = { 
            LLVMConstInt(LLVMInt64Type(), 0, false),
            LLVMConstInt(LLVMInt64Type(), i, false), 
        };
        LLVMValueRef indexAlloc = LLVMBuildGEP(
            irgen->builder,
            arrayAlloc,
            indices,
            sizeof(indices) / sizeof(LLVMValueRef),
            "tmp");

        LLVMBuildStore(
            irgen->builder,
            values[i],
            indexAlloc);
    }

    return arrayAlloc;
}

LLVMValueRef CompileIndexExp(Irgen *irgen, Exp *e) {
    assert(e->type == indexExp);

    LLVMValueRef alloc = GetAlloc(irgen, e);
    return LLVMBuildLoad(irgen->builder, alloc, "tmp");
}

LLVMValueRef CompileExp(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case literalExp:
            return CompileLiteralExp(irgen, e);
        case unaryExp:
            return CompileUnaryExp(irgen, e);
        case binaryExp:
            return CompileBinaryExp(irgen, e);
        case identExp:
            return CompileIdentExp(irgen, e);
        case callExp:
            return CompileCallExp(irgen, e); 
        case indexExp:
            return CompileIndexExp(irgen, e);
        case arrayExp:
            return CompileArrayExp(irgen, e);
        default:
            ASSERT(false, "Unknow expression type");
    }

    return NULL;
}