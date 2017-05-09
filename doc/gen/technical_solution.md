## Technical solution

### build/
This folder is were the make file produces the executable and library for the project.

#### CMakeLists.txt
This is the make file for the project
```cmake
cmake_minimum_required(VERSION 2.6)

# Use Clang
set(CMAKE_C_COMPILER /usr/bin/clang-3.9)
# set(CMAKE_C_COMPILER /usr/bin/clang++-3.9)
add_definitions("-fPIC")


project(Furlang)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Locate LLVM
message(STATUS "LLVM LIB PATH: ${LLVM_LIBRARIES_PATH}")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

set(LLVM_LIBS
    LLVMLTO
    LLVMObjCARCOpts
    LLVMSymbolize
    LLVMDebugInfoPDB
    LLVMDebugInfoDWARF
    LLVMXCoreDisassembler
    LLVMXCoreCodeGen
    LLVMXCoreDesc
    LLVMXCoreInfo
    LLVMXCoreAsmPrinter
    LLVMSystemZDisassembler
    LLVMSystemZCodeGen
    LLVMSystemZAsmParser
    LLVMSystemZDesc
    LLVMSystemZInfo
    LLVMSystemZAsmPrinter
    LLVMSparcDisassembler
    LLVMSparcCodeGen
    LLVMSparcAsmParser
    LLVMSparcDesc
    LLVMSparcInfo
    LLVMSparcAsmPrinter
    LLVMPowerPCDisassembler
    LLVMPowerPCCodeGen
    LLVMPowerPCAsmParser
    LLVMPowerPCDesc
    LLVMPowerPCInfo
    LLVMPowerPCAsmPrinter
    LLVMNVPTXCodeGen
    LLVMNVPTXDesc
    LLVMNVPTXInfo
    LLVMNVPTXAsmPrinter
    LLVMMSP430CodeGen
    LLVMMSP430Desc
    LLVMMSP430Info
    LLVMMSP430AsmPrinter
    LLVMMipsDisassembler
    LLVMMipsCodeGen
    LLVMMipsAsmParser
    LLVMMipsDesc
    LLVMMipsInfo
    LLVMMipsAsmPrinter
    LLVMHexagonDisassembler
    LLVMHexagonCodeGen
    LLVMHexagonAsmParser
    LLVMHexagonDesc
    LLVMHexagonInfo
    LLVMCppBackendCodeGen
    LLVMCppBackendInfo
    LLVMBPFCodeGen
    LLVMBPFDesc
    LLVMBPFInfo
    LLVMBPFAsmPrinter
    LLVMARMDisassembler
    LLVMARMCodeGen
    LLVMARMAsmParser
    LLVMARMDesc
    LLVMARMInfo
    LLVMARMAsmPrinter
    LLVMAMDGPUCodeGen
    LLVMAMDGPUAsmParser
    LLVMAMDGPUDesc
    LLVMAMDGPUUtils
    LLVMAMDGPUInfo
    LLVMAMDGPUAsmPrinter
    LLVMAArch64Disassembler
    LLVMAArch64CodeGen
    LLVMAArch64AsmParser
    LLVMAArch64Desc
    LLVMAArch64Info
    LLVMAArch64AsmPrinter
    LLVMAArch64Utils
    LLVMMIRParser
    LLVMLibDriver
    LLVMOption
    LLVMTableGen
    LLVMLineEditor
    LLVMX86Disassembler
    LLVMX86AsmParser
    LLVMX86CodeGen
    LLVMSelectionDAG
    LLVMAsmPrinter
    LLVMX86Desc
    LLVMMCDisassembler
    LLVMX86Info
    LLVMX86AsmPrinter
    LLVMX86Utils
    LLVMMCJIT
    LLVMPasses
    LLVMipo
    LLVMVectorize
    LLVMLinker
    LLVMIRReader
    LLVMAsmParser
    LLVMDebugInfoCodeView
    LLVMInterpreter
    LLVMCodeGen
    LLVMScalarOpts
    LLVMInstCombine
    LLVMInstrumentation
    LLVMProfileData
    LLVMBitWriter
    LLVMOrcJIT
    LLVMTransformUtils
    LLVMExecutionEngine
    LLVMTarget
    LLVMAnalysis
    LLVMRuntimeDyld
    LLVMObject
    LLVMMCParser
    LLVMBitReader
    LLVMMC
    LLVMCore
    LLVMSupport
)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Compiler library
add_library(furlib ../src/lib.c)
target_link_libraries(furlib ${LLVM_LIBS})
target_compile_options(furlib PRIVATE "-Werror")
target_compile_options(furlib PRIVATE "-std=c11")

# Compiler executable
add_executable(fur ../src/main.c)
target_link_libraries(fur ${LLVM_LIBS} furlib)
target_compile_options(fur PRIVATE "-Werror")
target_compile_options(fur PRIVATE "-std=c11")

# Test executable 
add_executable(fur_tests ../tests/test.cpp)
target_link_libraries(fur_tests ${GTEST_LIBRARIES} pthread ${LLVM_LIBS} furlib)
target_compile_options(fur_tests PRIVATE "-fpermissive") # required by GTEST
target_compile_options(fur_tests PRIVATE "-Werror")
```

### src/
This is the folder were all the source files for the compiler reside.

#### includes/all.h
Contains a few helpfull defines for the whole project.
```c
#pragma once

#include <assert.h>

#define ASSERT(cond, msg) assert(cond && msg)
#define bool int
#define true 1
#define false 0

#ifndef NULL
#define NULL (void *)0
#endif
```

#### includes/ast.h
Typedefs for all the ast nodes.
```c
#pragma once

#include "all.h"
#include "pool.h"
#include <llvm-c/Core.h>

struct Exp;
typedef struct Exp Exp;

struct Dcl;
typedef struct Dcl Dcl;

struct Smt;
typedef struct Smt Smt;

typedef struct {
	pool *dcl_pool;
	pool *smt_pool;
	pool *exp_pool;
	Dcl **dcls;
	int dclCount;
} ast_unit;

ast_unit *new_ast_unit();

typedef enum {
	badObj,
	pkgObj,
	constObj,
	typeObj,
	varObj,
	argObj,
	funcObj,
} ObjectType;

// Object represents a named construct such as a constant, type, varible, function.
typedef struct {
	ObjectType type;
	char *name;
	Dcl *node;
} Object;

// ============ Declarations ============

typedef enum {
	functionDcl,
	argumentDcl,
	varibleDcl,
} DclType;

struct Dcl {
	DclType type;
	LLVMValueRef llvmValue;
	union {
		struct { char *name; Exp *type; Exp *value; } 									varible;
		struct { Exp *type; char *name; } 												argument;
		struct { char *name; Dcl *args; int argCount; Exp *returnType; Smt *body; } 	function;
	};
};

Dcl *new_varible_dcl(ast_unit *ast, char *name, Exp *type, Exp *value);
Dcl *new_argument_dcl(ast_unit *ast, Exp *type, char *name);
Dcl *new_function_dcl(ast_unit *ast, char *name, Dcl *args, int argCount, Exp *returnType, Smt *body);

// ============ Statements ============

typedef enum {
	declareSmt,
	assignmentSmt,
	returnSmt,
	blockSmt,
	ifSmt,
	forSmt,
} SmtType;

struct Smt {
	SmtType type;
	union {
		Dcl *													declare;
		struct { Exp *left; Exp *right; } 						assignment;
		struct { Exp *result; } 								ret;
		struct { Smt *smts; int count; } 						block;
		struct { Exp *cond; Smt *body; Smt *elses; } 			ifs;
		struct { Dcl *index; Exp *cond; Smt *inc; Smt *body; } 	fors;
	};
};

Smt *new_declare_smt(ast_unit *ast, Dcl *dcl);
Smt *new_assignment_smt(ast_unit *ast, Exp *left, Exp *right);
Smt *new_binary_assignment_smt(ast_unit *ast, Exp *left, TokenType op, Exp *right);
Smt *new_ret_smt(ast_unit *ast, Exp *result);
Smt *new_block_smt(ast_unit *ast, Smt *smts, int smtCount);
Smt *new_if_smt(ast_unit *ast, Exp *cond, Smt *body, Smt *elses);
Smt *new_for_smt(ast_unit *ast, Dcl *index, Exp *cond, Smt *inc, Smt *body);

// ============ Expressions ============

typedef enum {
	identExp,
	literalExp,
	parenExp,
	selectorExp,
	braceLiteralExp,
	parenLiteralExp,
	indexExp,
	sliceExp,
	callExp,
	castExp,
	starExp,
	binaryExp,
	unaryExp,
	keyValueExp,
	keyValueListExp,
	structValueExp,
	arrayExp,
	arrayTypeExp,
	fieldTypeExp,
	structTypeExp,
} ExpType;

struct Exp {
	ExpType type;
	union {
		struct { char *name; Object *obj; } 				ident;
		Token 												literal;
		struct { Token op; Exp *right; } 					unary;
		struct { Exp *left; Token op; Exp *right; } 		binary;
		struct { Exp *exp; Exp *selector; } 				selector;
		struct { Exp *exp; Exp *index; } 					index;
		struct { Exp *exp; Exp *low; Exp *high; } 			slice;
		Exp *												star;
		struct { Exp *function; Exp *args; int argCount; } 	call;
		struct { Exp *key; Exp *value; } 					keyValue;
		struct { Exp *keyValues; int keyCount; } 			keyValueList;
		struct { Exp *type; Exp *list; } 					structValue;
		struct { Exp *values; int valueCount; } 			array;
		struct { Exp *type; Exp *length; } 					arrayType;
		struct { Exp *type; Exp *name; } 					fieldType;
		struct { Exp *fields; int feildCount; } 			structType;
	};
};

Exp *new_ident_exp(ast_unit *ast, char *ident);
Exp *new_literal_exp(ast_unit *ast, Token lit);
Exp *new_unary_exp(ast_unit *ast, Token op, Exp *right);
Exp *new_binary_exp(ast_unit *ast, Exp *left, Token op, Exp *right);
Exp *new_selector_exp(ast_unit *ast, Exp *exp, Exp* selector);
Exp *new_index_exp(ast_unit *ast, Exp *exp, Exp *index);
Exp *new_slice_exp(ast_unit *ast, Exp *exp, Exp *low, Exp *high);
Exp *new_star_exp(ast_unit *ast, Exp *exp);
Exp *new_call_exp(ast_unit *ast, Exp *function, Exp *args, int argCount);
Exp *new_key_value_exp(ast_unit *ast, Exp *key, Exp *value);
Exp *new_key_value_list_exp(ast_unit *ast, Exp *values, int keyCount);
Exp *new_struct_exp(ast_unit *ast, Exp *type, Exp *list);
Exp *new_array_exp(ast_unit *ast, Exp *values, int valueCount);
Exp *new_array_type_exp(ast_unit *ast, Exp *type, Exp *length);
Exp *new_feild_type_exp(ast_unit *ast, Exp *type, Exp *name);
Exp *new_struct_type_exp(ast_unit *ast, Exp *fields, int count);

```

#### ast.c
Constructors for the AST nodes.
```c
#include "includes/ast.h"

ast_unit *new_ast_unit() {
	ast_unit *ast = malloc(sizeof(ast_unit));
	ast->dcl_pool = new_pool(sizeof(Dcl), 128);
	ast->smt_pool = new_pool(sizeof(Smt), 128);
	ast->exp_pool = new_pool(sizeof(Exp), 128);
	ast->dcls = malloc(0);
	ast->dclCount = 0;

	return ast;
}

Exp *new_ident_exp(ast_unit *ast, char *ident) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = identExp;
	e->ident.name = ident;
	e->ident.obj = NULL;

	return e;
}

Exp *new_literal_exp(ast_unit *ast, Token lit) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = literalExp;
	e->literal = lit;

	return e;
}

Exp *new_unary_exp(ast_unit *ast, Token op, Exp *right) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = unaryExp;
	e->unary.op = op;
	e->unary.right = right;

	return e;
}

Exp *new_binary_exp(ast_unit *ast, Exp *left, Token op, Exp *right) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = binaryExp;
	e->binary.left = left;
	e->binary.op = op;
	e->binary.right = right;

	return e;
}

Exp *new_selector_exp(ast_unit *ast, Exp *exp, Exp* selector) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = selectorExp;
	e->selector.exp = exp;
	e->selector.selector = selector;

	return e;
}

Exp *new_index_exp(ast_unit *ast, Exp *exp, Exp *index) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = indexExp;
	e->index.exp = exp;
	e->index.index = index;

	return e;
}

Exp *new_slice_exp(ast_unit *ast, Exp *exp, Exp *low, Exp *high) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = sliceExp;
	e->slice.exp = exp;
	e->slice.low = low;
	e->slice.high = high;

	return e;
}

Exp *new_star_exp(ast_unit *ast, Exp *exp) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = starExp;
	e->star = exp;

	return e;
}

Exp *new_call_exp(ast_unit *ast, Exp *function, Exp *args, int argCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = callExp;
	e->call.function = function;
	e->call.args = args;
	e->call.argCount = argCount;

	return e;
}

Exp *new_key_value_exp(ast_unit *ast, Exp *key, Exp *value) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = keyValueExp;
	e->keyValue.key = key;
	e->keyValue.value = value;

	return e;
}

Exp *new_key_value_list_exp(ast_unit *ast, Exp *values, int keyCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = keyValueListExp;
	e->keyValueList.keyValues = values;
	e->keyValueList.keyCount = keyCount;

	return e;
}

Exp *new_struct_exp(ast_unit *ast, Exp *type, Exp *list) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = structValueExp;
	e->structValue.type = type;
	e->structValue.list = list;

	return e;
}

Exp *new_array_exp(ast_unit *ast, Exp *values, int valueCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = arrayExp;
	e->array.values = values;
	e->array.valueCount = valueCount;

	return e;
}

Exp *new_array_type_exp(ast_unit *ast, Exp *type, Exp *length) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = arrayTypeExp;
	e->arrayType.type = type;
	e->arrayType.length = length;

	return e;
}

Exp *new_feild_type_exp(ast_unit *ast, Exp *type, Exp *name) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = fieldTypeExp;
	e->fieldType.type = type;
	e->fieldType.name = name;

	return e;
}

Exp *new_struct_type_exp(ast_unit *ast, Exp *fields, int count) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = structTypeExp;
	e->structType.fields = fields;
	e->structType.feildCount = count;

	return e;
}

Smt *new_declare_smt(ast_unit *ast, Dcl *dcl) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = declareSmt;
	s->declare = dcl;

	return s; 
}

Smt *new_assignment_smt(ast_unit *ast, Exp *left, Exp *right) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = assignmentSmt;
	s->assignment.left = left;
	s->assignment.right = right;

	return s;
}

Smt *new_binary_assignment_smt(ast_unit *ast, Exp *left, TokenType op, Exp *right) {
	Smt *e = pool_get(ast->smt_pool);
	e->type = assignmentSmt;
	e->assignment.left = left;
	
	Token t = {ADD, 0, 0, ""};
	
	switch(op) {
		case ASSIGN:
			e->assignment.right = right;
			return e;
		case ADD_ASSIGN:
			t.type = ADD;
			break;
		case SUB_ASSIGN:
			t.type = SUB; 
			break;
		case MUL_ASSIGN:
			t.type = MUL;
			break;
		case REM_ASSIGN:
			t.type = REM;
			break;
		case OR_ASSIGN:
			t.type = OR; 
			break;
		case SHL_ASSIGN:
			t.type = SHL;
			break;
		default:
			ASSERT(false, "Expected an assignment token");
	}

	e->assignment.right = new_binary_exp(ast, left, t, right);
	return e; 
}

Smt *new_ret_smt(ast_unit *ast, Exp *result) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = returnSmt;
	s->ret.result = result;
	
	return s;
}

Smt *new_block_smt(ast_unit *ast, Smt *smts, int smtCount) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = blockSmt;
	s->block.smts = smts;
	s->block.count = smtCount;

	return s; 
}

Smt *new_if_smt(ast_unit *ast, Exp *cond, Smt *body, Smt *elses) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = ifSmt;
	s->ifs.cond = cond;
	s->ifs.body = body;
	s->ifs.elses = elses;

	return s;
}

Smt *new_for_smt(ast_unit *ast, Dcl *index, Exp *cond, Smt *inc, Smt *body) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = forSmt;
	s->fors.index = index;
	s->fors.cond = cond;
	s->fors.inc = inc;
	s->fors.body = body;

	return s;
}

Dcl *new_varible_dcl(ast_unit *ast, char *name, Exp *type, Exp *value) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = varibleDcl;
	d->varible.name = name;
	d->varible.type = type;
	d->varible.value = value;

	return d;
}

Dcl *new_argument_dcl(ast_unit *ast, Exp *type, char *name) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = argumentDcl;
	d->argument.type = type;
	d->argument.name = name;

	return d;
}

Dcl *new_function_dcl(ast_unit *ast, char *name, Dcl *args, int argCount, Exp *returnType, Smt *body) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = functionDcl;
	d->function.name = name;
	d->function.args = args;
	d->function.argCount = argCount;
	d->function.returnType = returnType;
	d->function.body = body;

	return d;
}

```

#### includes/error.h
```c
#pragma once

#include "all.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void error(char *src, int line, int start, int end, char *msg, ...);
```

#### error.c
Pretty prints errors to the terminal.
```c
#include "includes/error.h"

void verror(char *src, int line, int start, int end, char *msg, va_list args) {
    ASSERT(start >= 0, "start underflows the line");
    
    // print message
    printf("\e[31m\e[1mERROR:\e[0m "); 
    vprintf(msg, args);
    printf("\n\n");

    // get the position of line withing the source code
    char *src_ptr = src;
    int line_end = 0;
    int line_start = 0;
    int current_line = 0;
    while(true) {
        if(*src_ptr == '\n') {
            line_end = src_ptr - src;
            if(current_line != line - 1) {
                line_start = src_ptr - src;
            } else {
                break;
            }
            current_line++;
        }
        src_ptr++;
    }
    int line_length = line_end - line_start;
    
    // print line number
    printf("\e[2m%5d|\e[0m ", line);
    
    // print source code
    char *src_line = malloc(line_length);
    memcpy(src_line, src + line_start, line_length * sizeof(char));
    if(src_line[0] == '\n') {
        // remove leading newline
        src_line++;
        line_length--;
    }
    while(src_line[0] == '\t') {
        // print tabs as 4 spaces (since we dont start on a column boundry)
        printf("    ");
        src_line++;
        line_length += 4;
    }
    printf("%s\n", src_line);

    // print error underlining
    char *buff = malloc(line_length * sizeof(char));
    for (int i = 0; i < line_length; i++) {
        if (i >= start - 1 && i < end) {
            buff[i] = '^';
        } else {
            buff[i] = ' ';
        }
    }
    buff[line_length-1] = '\0';
    printf("       \e[91m\e[1m%s\e[0m\n\n", buff);
}

void error(char *src, int line, int start, int end, char *msg, ...) {
    va_list args;
    va_start(args, msg);
    verror(src, line, start, end, msg, args);
    va_end(args);
}
```

#### includes/irgen.h
```c
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
```

#### irgen.c
```c
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
```

#### includes/lexer.h
```c
#pragma once

#include "all.h"
#include <string.h>
#include <stdlib.h>

typedef enum {
	ILLEGAL,
	IDENT,
	
	// literals
	INT,
	FLOAT,
	HEX,
	OCTAL,
	STRING,

	// key words
	BREAK,
	CASE,
	CONST,
	CONTINUE,
	DEFAULT,
	DEFER,
	ELSE,
	FALLTHROUGH,
	FOR,
	FUNC,
	PROC,
	IF,
	IMPORT,
	RETURN,
	SELECT,
	STRUCT,
	SWITCH,
	TYPE,
	VAR,

	// symbols
	SEMI,
	COLON,
	DEFINE,
	DOUBLE_COLON,
	ELLIPSE,
	PERIOD,
	COMMA,
	LPAREN,
	RPAREN,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	ADD,
	ADD_ASSIGN,
	INC,
	ARROW,
	SUB,
	SUB_ASSIGN,
	DEC,
	MUL,
	MUL_ASSIGN,
	QUO,
	QUO_ASSIGN,
	REM,
	REM_ASSIGN,
	XOR,
	XOR_ASSIGN,
	LSS,
	LEQ,
	SHL,
	SHL_ASSIGN,
	GTR,
	GEQ,
	SHR,
	SHR_ASSIGN,
	ASSIGN,
	EQL,
	NOT,
	NEQ,
	AND_NOT,
	AND_NOT_ASSIGN,
	AND,
	AND_ASSIGN,
	LAND,
	OR,
	OR_ASSIGN,
	LOR,
	
	// eof token
	END,
} TokenType;

typedef struct {
	TokenType type;
	int line;
	int column;
	char *value;
} Token;

typedef struct {
	char *source;
	int line;
	int column;
	bool semi;
} Lexer;

Token *Lex(char *source);
char *TokenName(TokenType type);
char *GetLine(char *src, int line);
int get_binding_power(TokenType type);
```

#### lexer.c
```c
#include "includes/lexer.h"

// Removes spaces, newlines and tabs
void clearWhitespace(Lexer *lexer) {
	while (*lexer->source == ' ' ||
		*lexer->source == '\t' ||
		(*lexer->source == '\n' && !lexer->semi) ||
		*lexer->source == '\r') {

		lexer->source++;
		lexer->column++;
	}
}

// checks if the character is a digit 0-9
bool isDigit(char *input) {
	return *input >= '0' && *input <= '9';
}

// checks if the character is a letter a-z or A-Z
bool isLetter(char *input) {
	return (*input >= 'a' && *input <= 'z') || 
		(*input >= 'A' && *input <= 'Z');
}

// returns the character as an integer
int asDigit(char *input) {
	if ('0' <= *input && *input <= '9') {
		return *input - '0';
	} else if('a' <= *input && *input <= 'f') {
		return *input - 'a' + 10;
	} else if('A' <= *input && *input <= 'F') {
		return *input - 'A' + 10;
	}

	return -1; // unrecognised digit
}

// returns the word at the start of the string
char *word(Lexer *lexer) {
	// token is an ident
	char *word = (char *)malloc(1024);

	// copy all letter characters
	char *wordPtr = word;
	int length = 0;
	do {
		*wordPtr++ = *lexer->source;
		lexer->source++;
		lexer->column++;
		length++;
	} while (isLetter(lexer->source) || isDigit(lexer->source));

	// null-terminate
	*wordPtr = '\0';
	length++;

	// shrink string to ident length
	word = realloc(word, length);

	return word;
}

// Extracts the mantiass from input into number, returning the characters written.
int extractMantissa(char **input, char **number, int base) {
	int length = 0;
	while(isDigit(*input) && asDigit(*input) < base) {
		**number = **input;
		length++;
		(*input)++;
		(*number)++;
	}

	return length;
}

// extracts the number at the start of the input string, returns error code
char *number(Lexer *lexer, TokenType *type) {
	char *number = (char *)malloc(1024 * sizeof(char));
	char *numberPtr = number;
	
	*type = INT;
	int base = 10;
	int length = 0; //amount of characters in number

	if (*lexer->source == '0') {
		lexer->source++;
		if (*lexer->source == 'x' || *lexer->source == 'X') {
			// number is hexadecimal
			*type = HEX;
			base = 16;
			lexer->source++; // skip 'x' / 'X'
			length += extractMantissa(&lexer->source, &numberPtr, base);
		} else {
			// assume number is octal
			bool octal = true;
			base = 8;
			length += extractMantissa(&lexer->source, &numberPtr, base);
			
			if (*lexer->source == '8' || *lexer->source == '9') {
				// number was not octal
				octal = false;
				base = 10;
				length += extractMantissa(&lexer->source, &numberPtr, base);
			}
			else {
				*type = OCTAL;
			}

			if (*lexer->source == '.') {
				// number is fractional
				goto fraction;
			}

			if (!octal) {
				// illegal octal number
				return "";
			}
		}
		goto exit;
	}

	base = 10;
	length += extractMantissa(&lexer->source, &numberPtr, base);

fraction:
	if (*lexer->source == '.') {
		// copy '.'
		*numberPtr++ = '.';
		lexer->source++; 
		length++;
		
		*type = FLOAT;
		base = 10;
		length += extractMantissa(&lexer->source, &numberPtr, base);
	}

exit:
	// Null terminate
	length++;
	*numberPtr = '\0';

	number = realloc(number, length * sizeof(char));
	return number;
}

char *escape(char **input, char quote) {
	char *esc = (char *)malloc(1024 * sizeof(char));
	int length = 0;
	int n = 0;
	int base = 0;
	int max = 0;

	// copy '/'
	*esc++ = '/';
	length++;
	(*input)++;

	if (**input == quote) {
		(*input)++;
	}
	 
	switch (**input) {
	case 'a':
	case 'b': 
	case 'f': 
	case 'n':
	case 'r':
	case 't':
	case 'v':
	case '\\':
		*esc++ = **input;
		length++;
		(*input)++;
		break;
	
	// octal
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		n = 3;
		base = 8;
		max = 255;
		break;

	// hex
	case 'x':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 2;
		base = 16;
		max = 255;
		break;

	// small unicode
	case 'u':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 4;
		base = 16;
		max = 0x0010FFFF;
		break;

	// full unicode
	case 'U':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 8;
		base = 16;
		max = 0x0010FFFF;
		break;

	default:
		// unkown escape sequence
		break;

	}

	int x = 0;
	while (n > 0) {
		int d = asDigit(*input);
		if (d >= base) {
			// illegal character
		}

		x = x * base + d;
		*esc++ = **input;
		length++;
		(*input)++;
		n--;
	}

	// check if unicode character is valid
	if (x > max || (0xD800 <= x && x < 0xE000)) {
		// invalid unicode code point
	}

	// null terminate
	*esc = '\0';
	length++;

	esc = realloc(esc, length * sizeof(char));
	return esc;
}

char *lex_string(char **input) {
	char *string = (char *)malloc(1024 * sizeof(char));
	int length = 0;

	(*input)++; // skip '"'
	char *stringPtr = string;
	while (**input != '"') {
		if (**input == '\\') {
			char *esc = escape(input, '"');
			while (*esc) {
				*stringPtr++ = *esc++;
				length++;
			}
		}

		*stringPtr++ = **input;
		length++;
		(*input)++;
	}
	(*input)++; // skip '"'
	
	// null terminate
	*stringPtr = '\0';
	length++;

	string = realloc(string, length * sizeof(char));
	return string;
}

TokenType switch2(char **input, TokenType token0, TokenType token1) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	return token0;
}

TokenType switch3(char **input, TokenType token0, TokenType token1, char chr, TokenType token2) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		return token2;
	} 
	return token0;
}

TokenType switch4(char **input, TokenType token0, TokenType token1, char chr, TokenType token2, TokenType token3) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		if (**input == '=') {
			(*input)++;
			return token3;
		}	
		return token2;
	} 
	return token0;
}

// returns the second character in a string
char peek(char *input) {
	return *(++input);
}

void next(Lexer *lexer) {
	lexer->source++;
	lexer->column++;
}

// finds the token type for a word
TokenType keyword(char *word) {
	if (strcmp(word, "break") == 0) return BREAK;
	if (strcmp(word, "case") == 0) return CASE;
	if (strcmp(word, "const") == 0) return CONST;
	if (strcmp(word, "continue") == 0) return CONTINUE;
	if (strcmp(word, "default") == 0) return DEFAULT;
	if (strcmp(word, "defer") == 0) return DEFER;
	if (strcmp(word, "else") == 0) return ELSE;
	if (strcmp(word, "fallthrough") == 0) return FALLTHROUGH;
	if (strcmp(word, "for") == 0) return FOR;
	if (strcmp(word, "func") == 0) return FUNC;
	if (strcmp(word, "proc") == 0) return PROC;
	if (strcmp(word, "if") == 0) return IF;
	if (strcmp(word, "import") == 0) return IMPORT;
	if (strcmp(word, "return") == 0) return RETURN;
	if (strcmp(word, "select") == 0) return SELECT;
	if (strcmp(word, "struct") == 0) return STRUCT;
	if (strcmp(word, "switch") == 0) return SWITCH;
	if (strcmp(word, "type") == 0) return TYPE;
	if (strcmp(word, "var") == 0) return VAR;
	return IDENT;
}

Token *Lex(char *source) {
	Lexer lexer = {source, 1, 1, false};

	Token *tokens = (Token *)malloc(0);

	int i = 0;
	while (*lexer.source) {
		i++;
		clearWhitespace(&lexer);

		Token token;
		token.line = lexer.line;
		token.column = lexer.column;
		token.value = "";

		if (isLetter(lexer.source)) {
			// token is an identifier
			token.value = word(&lexer);
			token.type = keyword(token.value);
			if (token.type == IDENT || 
				token.type == BREAK || 
				token.type == CONTINUE || 
				token.type == FALLTHROUGH || 
				token.type == RETURN) {
			
				lexer.semi = true;
			}
		}
		else if (isDigit(lexer.source)) {
			// token is a number
			lexer.semi = true;
			token.type = INT;
			token.value = number(&lexer, &token.type);
		} else {
			// token is a symbol
			switch (*lexer.source) {
				case '\n':
					token.type = SEMI;
					lexer.semi = false;
					lexer.column = 1;
					lexer.line++;
					next(&lexer);
					break;

				case '"':
					lexer.semi = true;
					token.type = STRING;
					token.value = lex_string(&lexer.source);
					break;

				case ':':
					token.type = switch3(&lexer.source, COLON, DEFINE, ':', DOUBLE_COLON);
					break;

				case '.':
					token.type = PERIOD;
					next(&lexer);
					if (*lexer.source == '.') {
						next(&lexer);
						if (*lexer.source == '.') {
							next(&lexer);
							token.type = ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = COMMA;
					next(&lexer);
					break;

				case ';':
					token.type = SEMI;
					next(&lexer);
					break;

				case '(':
					token.type = LPAREN;
					next(&lexer);
					break;

				case ')':
					lexer.semi = true;
					token.type = RPAREN;
					next(&lexer);
					break;

				case '[':
					token.type = LBRACK;
					next(&lexer);
					break;

				case ']':
					lexer.semi = true;
					token.type = RBRACK;
					next(&lexer);
					break;

				case '{':
					lexer.semi = false;
					token.type = LBRACE;
					next(&lexer);
					break;

				case '}':
					lexer.semi = true;
					token.type = RBRACE;
					next(&lexer);
					break;

				case '+':
					token.type = switch3(&lexer.source, ADD, ADD_ASSIGN, '+', INC);
					if (token.type == INC) lexer.semi = true;
					break;

				case '-':
					if (peek(lexer.source) == '>') {
						token.type = ARROW;
						lexer.source += 2;
					} else {
						token.type = switch3(&lexer.source, SUB, SUB_ASSIGN, '-', DEC);
						if (token.type == DEC) lexer.semi = true;
					}
					break;

				case '*':
					token.type = switch2(&lexer.source, MUL, MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(&lexer.source, QUO, QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(&lexer.source, REM, REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(&lexer.source, XOR, XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(&lexer.source, LSS, LEQ, '<', SHL, SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(&lexer.source, GTR, GEQ, '>', SHR, SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(&lexer.source, ASSIGN, EQL);
					break;

				case '!':
					token.type = switch2(&lexer.source, NOT, NEQ);
					break;

				case '&':
					if (peek(lexer.source) == '^') {
						next(&lexer);
						token.type = switch2(&lexer.source, AND_NOT, AND_NOT_ASSIGN);
					} else {
						token.type = switch3(&lexer.source, AND, AND_ASSIGN, '&', LAND);
					}
					break;

				case '|':
					token.type = switch3(&lexer.source, OR, OR_ASSIGN, '|', LOR);
					break;

				default:
					token.type = ILLEGAL;
					next(&lexer);
					break;
			}
		}

		if (token.value == NULL) token.value = TokenName(token.type);

		// Add the token to the array
		tokens = (Token *)realloc(tokens, (i + 1) * sizeof(Token));
		tokens[i - 1] = token;

		// Check for newline
		if (*lexer.source == '\n') {
			lexer.line++;
		}

	}

	// End of file token
	Token token;
	token.column = 321;
	token.line = 321;
	token.value = "";
	token.type = END;
	tokens[i] = token;

	return tokens;
}

char *TokenName(TokenType type) {
	switch(type) {
		case ILLEGAL: return "illegal";
		case END: return "[END]";
		case IDENT: return "[ident]";
		
		case INT: return "[int]";
		case FLOAT: return "[float]";
		case HEX: return "[hex]";
		case OCTAL: return "[octal]";
		case STRING: return "[string]";
		
		case BREAK: return "break";
		case CASE: return "case";
		case CONST: return "const";
		case CONTINUE: return "continue";
		case DEFAULT: return "default";
		case ELSE: return "else";
		case FALLTHROUGH: return "fallthrough";
		case FOR: return "for";
		case FUNC: return "func";
		case PROC: return "proc";
		case IF: return "if";
		case IMPORT: return "import";
		case RETURN: return "return";
		case SELECT: return "select";
		case STRUCT: return "struct";
		case SWITCH: return "switch";
		case TYPE: return "type";
		case VAR: return "var";
		case DEFER: return "defer";

		case DEFINE: return ":=";
		case SEMI: return ";";
		case COLON: return ":";
		case DOUBLE_COLON: return "::";
		case ELLIPSE: return "...";
		case PERIOD: return ".";
		case COMMA: return ",";
		case LPAREN: return "(";
		case RPAREN: return ")";
		case LBRACK: return "[";
		case RBRACK: return "]";
		case LBRACE: return "{";
		case RBRACE: return "}";
		case ADD: return "+";
		case ADD_ASSIGN: return "+=";
		case INC: return "++";
		case ARROW: return "->";
		case SUB: return "-";
		case SUB_ASSIGN: return "-=";
		case DEC: return "--";
		case MUL: return "*";
		case MUL_ASSIGN: return "*=";
		case QUO: return "/";
		case QUO_ASSIGN: return "/=";
		case REM: return "%";
		case REM_ASSIGN: return "%=";
		case XOR: return "^";
		case XOR_ASSIGN: return "^=";
		case GTR: return ">";
		case GEQ: return ">=";
		case LSS: return "<";
		case LEQ: return "<=";
		case SHL: return "<<";
		case SHL_ASSIGN: return "<<=";
		case SHR: return ">>";
		case SHR_ASSIGN: return ">>=";
		case ASSIGN: return "=";
		case EQL: return "==";
		case NOT: return "!";
		case NEQ: return "!=";
		case AND: return "&";
		case AND_ASSIGN: return "&=";
		case AND_NOT: return "&^";
		case AND_NOT_ASSIGN: return "&^=";
		case LAND: return "&&";
		case OR: return "|";
		case OR_ASSIGN: return "|=";
		case LOR: return "||";
	}

	return "UNKOWN_NAME";
}

char *GetLine(char *source, int line) {
	int currentLine = 1;

	while(*source != '\0') {
		if (*(source++) == '\n') currentLine++;
		
		if (currentLine == line) {
			// find the line length
			int lineLength = 0;
			while(*source && *source != '\n') {
				source++;
				lineLength++;
			}

			// copy the line to a buffer and return
			char *buff = malloc((lineLength+1) * sizeof(char));
			memcpy(buff, source - lineLength, lineLength);
			buff[lineLength] = '\0';
			return buff;
		}
	}

	return "LINE_NOT_FOUND";
}

// get_binding_power returns the left binding power of a token
int get_binding_power(TokenType type) {
	switch (type) {
	case END:
		return -10;
	// Non-binding operators
	case SEMI:
		return 0;
	// Assignment operators
	case ASSIGN:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case REM_ASSIGN:
	case OR_ASSIGN:
	case SHR_ASSIGN:
	case DEFINE:
		return 10;
	// Logical operators
	case LAND:
	case LOR:
		return 20;
	// Equality operators
	case EQL:
	case NEQ:
	case LSS:
	case GTR:
	case LEQ:
	case GEQ:
		return 30;
	// Math operators
	case ADD:
	case SUB:
		return 40;
	case MUL:
	case QUO:
	case REM:
		return 50;
	// Special unary
	case NOT:
		return 60;
	// Strongly bound
	case PERIOD:
	case LBRACK:
	case LPAREN:
		return 70;
	// Unknow token
	default:
		return 0;
	}
}
```

#### lib.c
Includes all the source files so the compiler can be exposed as a library (which we use in the unit tests).
```c
#include "error.c"
#include "lexer.c"
#include "ast.c"
#include "parser.c"
#include "irgen.c"
#include "pool.c"
#include "queue.c"
#include "string.c"
```

#### main.c
Entry point for the compiler
```c
#include "includes/error.h"
#include "includes/lexer.h"
#include "includes/ast.h"
#include "includes/parser.h"
#include "includes/irgen.h"
#include "includes/parser.h"
#include "includes/string.h"


#include <llvm-c/BitWriter.h>
#include <stdlib.h>

void print_usage() {
	printf("usage: fur filename <flags>\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	string in_file = string_new(argv[1]);
	string out_file = string_new("");
	bool emit_tokens = false;
	bool emit_ircode = false;

	// Check for no/incorrect input file
	if (argc < 2 || argv[0][0] == '-') print_usage();
	
	// Check input file has .fur extension
	int in_len = string_length(in_file);
	if(in_len < 4) print_usage();
	if(strcmp(in_file + in_len - 4, ".fur") != 0) print_usage();
	
	// Create default out name
	out_file = string_append_length(out_file, in_file, in_len - 4);
	out_file = string_append_cstring(out_file, ".ll");
	
	// Parse tokens
	for (int i = 2; i < argc; i++) {
		if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
			if(i == argc - 1) print_usage();
			i++;
			if(argv[i][0] == '-') print_usage();
			out_file = argv[i];
			i++;
		} else {
			emit_tokens = emit_tokens || strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tokens") == 0;
			emit_ircode = emit_ircode || strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ircode") == 0;
		}
	}

	// Open the in and out files
	FILE *in_file_hdl = fopen(in_file, "r");
	FILE *out_file_hdl = fopen(out_file, "w+");

	// Copy file content to string buffer
	string buffer = string_new_file(in_file_hdl);
	printf("Done\n");

	// Compile the file
	Token *tokens = Lex(buffer);
	printf("Lexer done\n");
	parser *p = new_parser(tokens);
	ast_unit *ast = parse_file(p);
	printf("Parser done\n");
	Irgen *irgen = NewIrgen();
	printf("Irgen done\n");
	for (int i = 0; i < ast->dclCount; i++) {
        CompileFunction(irgen, ast->dcls[i]);
    }
	printf("Compiled to LLVM\n");

	// Write the file to llvm bitcode
	int rc = LLVMWriteBitcodeToFD(irgen->module, fileno(out_file_hdl), true, true); // out_file_hdl closed here
	if (rc > 0) {
		printf("LLVM to bitcode error\n");
		exit(1);
	}
	printf("Compile to bitcode\n");
	
	// Compile to assembly
	string llc_command = string_new("llc-3.9 ");
	llc_command = string_append(llc_command, out_file);
	system(llc_command);
	printf("Compiled to assembly\n");
	string llc_file = string_copy(out_file);
	llc_file = string_slice(llc_file, 0, string_length(llc_file) - 3); // remove .ll
	llc_file = string_append_cstring(llc_file, ".s"); // add .s

	// Create executable
	string clang_command = string_new("clang-3.9 ");
	clang_command = string_append(clang_command, llc_file);
	system(clang_command);
	printf("Executable created\n");

	// Remove temporary files
	string rm_command = string_new("rm ");
	rm_command = string_append(rm_command, out_file);
	rm_command = string_append_cstring(rm_command, " ");
	rm_command = string_append(rm_command, llc_file);
	system(rm_command);
	printf("Removed temporary files\n");

	fclose(in_file_hdl);

	return 0;
}
```

#### includes/parser.h
```c
#pragma once

#include "uthash.h"
#include "all.h"
#include "queue.h"

#define ERROR_QUEUE_SIZE 10
#define MAX_ERRORS 10

typedef struct {
	char *name;
	Object *obj;
	UT_hash_handle hh;
} scope_object;

struct _scope {
	struct _scope *outer;
	scope_object *objects;
};
typedef struct _scope scope;

typedef struct {
	scope *scope;
    Token *tokens;
	ast_unit *ast;
	queue *error_queue;
} parser;

typedef enum {
	parser_error_expect_token,
	parser_error_expect_declaration,
	parser_error_expect_statement,
	parser_error_expect_expression,
	parser_error_expect_type,
	parser_error_expect_array_length,
	parser_error_expect_block,
	parser_error_expect_prefix,
	parser_error_expect_infix,
} parser_error_type;

typedef struct {
	parser_error_type type;
	Token *start;
	int length;

	union {
		struct {
			TokenType type;
		} expect_token;
	};
} parser_error;

// Parser interface
parser *new_parser(Token *tokens);
ast_unit *parse_file(parser *parser);

// Scope
scope *parser_new_scope(scope *outer);
void parser_enter_scope(parser *parser);
void parser_exit_scope(parser *parser);
bool parser_insert_scope(parser *parser, char *name, Object *object);
Object *parser_find_scope(parser *parser, char *name);

// Helpers
void parser_next(parser *parser);
Token *parser_expect(parser *parser, TokenType type);
void parser_expect_semi(parser *parser);
parser_error *new_error(parser *p, parser_error_type type, int length);
parser_error *new_error_token(parser *p, TokenType token_type);

// Declarations
Dcl *parse_declaration(parser *parser);
Dcl *parse_function_dcl(parser *parser);
Dcl *parse_variable_dcl(parser *parser);

// Statements
Smt *parse_statement(parser *parser);
Smt *parse_statement_from_string(char *src);
Smt *parse_block_smt(parser *p);
Smt *smtd(parser *p, Token *token);

// Expressions
Exp *parse_expression(parser *parser, int rbp);
Exp *parse_expression_from_string(char *src);
Exp *nud(parser *parser, Token *token);
Exp *led(parser *parser, Token *token, Exp *exp);
Exp *parse_key_value_exp(parser *parser);
Exp *parse_key_value_list_exp(parser *parser);
Exp *parse_array_exp(parser *parser);
Exp *parse_type(parser *parser);
Exp *parse_ident_exp_from_token(parser *parser, Token *token);
Exp *parse_ident_exp(parser *parser);
```

#### parser.c
```c
#include "includes/parser.h"

// new_parser creates a new parser
parser *new_parser(Token *tokens) {
	parser *p = (parser *)malloc(sizeof(parser));
	p->tokens = tokens;
	p->scope = parser_new_scope(NULL);
	p->ast = new_ast_unit();
	p->error_queue = new_queue(sizeof(parser_error));
	return p;
}

// parse_file creates an abstract sytax tree from the tokens in parser 
ast_unit *parse_file(parser *p) {
	Dcl **dcls = malloc(0);
	int dclCount = 0;
	while(p->tokens->type != END) {
		Dcl *d = parse_declaration(p);
		dcls = realloc(dcls, ++dclCount * sizeof(Dcl *));
		memcpy(dcls + dclCount - 1, &d, sizeof(Dcl *));
	}

	ast_unit *f = malloc(sizeof(ast_unit));
	f->dcls = dcls;
	f->dclCount = dclCount;

	return f;
}

// creates a new empty scope
scope *parser_new_scope(scope *outer) {
	scope *inner_scope = (scope *)malloc(sizeof(scope));
	inner_scope->outer = outer;
	scope_object *objects = NULL;
	inner_scope->objects = objects;

	return inner_scope;
}

// parser_enter_scope enters a new inner scope
void parser_enter_scope(parser *p) {
	p->scope = parser_new_scope(p->scope);
}

// parser_exit_scope exits the current scope
void parser_exit_scope(parser *p) {
	// clear hash table and free all scope objects
	scope_object *obj, *tmp;
	HASH_ITER(hh, p->scope->objects, obj, tmp) {
		HASH_DEL(p->scope->objects, obj);
		free(obj);
	}

	// Move to outer scipe
	scope *outer = p->scope->outer;
	free(p->scope);
	p->scope = outer;
}

// parser_insert_scope inserts an object into the current scope
bool parser_insert_scope(parser *p, char *name, Object *object) {
	// check if name is already in scope
	scope_object *obj;
	HASH_FIND_STR(p->scope->objects, name, obj);
	if (obj != NULL) return false;

	// add object to scope
	obj = (scope_object *)malloc(sizeof(scope_object));
	obj->name = name;
	obj->obj = object;
	HASH_ADD_KEYPTR(hh, p->scope->objects, obj->name, strlen(obj->name), obj);
	return true;
}

// parser_find_scope finds an object in scope
Object *parser_find_scope(parser *p, char *name) {
	scope_object *obj;
	for (scope *scope = p->scope; scope != NULL; scope = scope->outer) {
		HASH_FIND_STR(scope->objects, name, obj);
		if (obj != NULL) return obj->obj;
	}

	return NULL;
}

// parser_next moves the parser onto the next token
void parser_next(parser *p) {
	p->tokens++;
}

// parser_expect checks that the current token is of type type, if true parser advances, 
// else an error message is created.
Token *parser_expect(parser *p, TokenType type) {
	Token *token = p->tokens;
	if(token->type == type) {
		parser_next(p);
		return token;
	} else {
		new_error_token(p, type);
		return NULL;
	}
}

// parser_expect_semi expects a semicolon
void parser_expect_semi(parser *p) {
	if(p->tokens->type == SEMI || p->tokens->type == END) {
		parser_next(p);
	} else {
		new_error_token(p, SEMI);
	}
}

// new_error added a new error to the queue
parser_error *new_error(parser *p, parser_error_type type, int length) {
	parser_error *error = queue_push_back(p->error_queue);
	error->type = type;
	error->start = p->tokens;
	error->length = length;
	return error;
}

// new_error_token added a new token error to the queue
parser_error *new_error_token(parser *p, TokenType token_type) {
	parser_error *error = new_error(p, parser_error_expect_token, 1);
	error->expect_token.type = token_type;
	return error;
}

char *format_error(char *src, parser_error *error) {
	return "";
}

// parse_declaration parse a decleration node
Dcl *parse_declaration(parser *p) {
	switch(p->tokens->type) {
		case PROC:
			return parse_function_dcl(p);
		case VAR:
		case IDENT:
			return parse_variable_dcl(p);
		default: {
			// expected a top level declaration
			new_error(p, parser_error_expect_declaration, 1);
			return NULL;
		}
	}
}

Dcl *parse_declaration_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_declaration(p);
}

void parser_skip_next_block(parser *p) {
		// Move to start of block
		while(p->tokens->type != LBRACE) p->tokens++;
		
		// Skip over block (and all sub blocks)
		int depth = 0;
		do {
			if(p->tokens->type == LBRACE) depth++;
			else if(p->tokens->type == RBRACE) depth--;
			p->tokens++;
		} while(depth > 0);

		if(p->tokens->type == SEMI) p->tokens++;
}

void parser_skip_to_semi(parser *p) {
	// Move past first semi
	while(p->tokens->type != SEMI && p->tokens->type != END) p->tokens++;
	if(p->tokens->type == SEMI) p->tokens++;
}

// parse_function_dcl parses a function decleration
Dcl *parse_function_dcl(parser *p) {
	// Parse proc
	Token *proc = parser_expect(p, PROC);
	if (proc == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// Parse function name
	Token *ident = parser_expect(p, IDENT);
	if (ident == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}
	char *name = ident->value; // function name
	
	// Parse argument seperator
	parser_expect(p, DOUBLE_COLON);
	// missing double colon is not fatel so countinue

	// Parse arguments
	Dcl *args = (Dcl *)malloc(0);
	int argCount = 0;
	while(p->tokens->type != ARROW && p->tokens->type != LBRACE) {
		if (argCount > 0) parser_expect(p, COMMA);
		// missing comma not fatel

		args = realloc(args, sizeof(Dcl) * ++argCount);

		// Construct argument
		Exp *type = parse_type(p); // arg type
		if (type == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}

		// arg name
		Token *name_token = parser_expect(p, IDENT); 
		if (name_token == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}
		char *name = name_token->value; 

		// add argument to list
		Dcl *arg = new_argument_dcl(p->ast, type, name);
		void *dest = memcpy(args + argCount - 1, arg, sizeof(Dcl));
	}
	
	Token *arrow = parser_expect(p, ARROW);
	if (arrow == NULL) {
		// arrow fatel since we dont know the return type
		parser_skip_next_block(p);
		return NULL;
	}

	Exp *return_type = parse_type(p);
	if (return_type == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// insert arguments into scope
	for (int i = 0; i < argCount; i++) {
		// insert into scope
		Object *obj = (Object *)malloc(sizeof(Object));
		obj->name = args[i].argument.name;
		obj->node = args + i;
		obj->type = argObj;
		parser_insert_scope(p, obj->name, obj);
	}

	// insert function into scope
	Dcl* function = new_function_dcl(p->ast, name, args, argCount, return_type, NULL);
	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = function;
	obj->type = funcObj;
	parser_insert_scope(p, name, obj);
	
	// parse body
	Smt *body = parse_block_smt(p);
	function->function.body = body;

	if(p->tokens->type == SEMI) p->tokens++;

	return function;
}

Dcl *parse_variable_dcl(parser *p) {
	char *name;
	Exp *type = NULL;
	Exp *value;

	if(p->tokens->type == VAR) {
		p->tokens++;
		
		// Type
		type = parse_type(p);
		if (type == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}

		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;

		// Assign
		parser_expect(p, ASSIGN);
		// non fatel

		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	} else {
		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;
		
		// Define
		parser_expect(p, DEFINE);
		// non fatel
		
		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	}

	Dcl *dcl = new_varible_dcl(p->ast, name, type, value);

	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = dcl;
	obj->type = varObj;
	parser_insert_scope(p, name, obj);

	return dcl;
}

// Parses the next statement by calling smtd on the first token else handle
// the declaration/assignment
Smt *parse_statement(parser *p) {
	Token *t = p->tokens;
	Smt *smt = smtd(p, t);
	if (smt != NULL) {
		return smt;
	}

	// Statement is an assignment/declaration, so treat it like an expression
	// and transform it.
	Exp *exp = parse_expression(p, 0);
	if(exp == NULL || exp->type != binaryExp) {
		// expected assigment/declation statement
		if(exp == NULL) queue_pop_back(p->error_queue); // remove expression error
		new_error(p, parser_error_expect_statement, 1);
		parser_skip_to_semi(p);
	} 
	
	Exp *left = exp->binary.left;
	Exp *right = exp->binary.right;
	Token op = exp->binary.op;

	switch(op.type) {
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN:
			smt = new_binary_assignment_smt(p->ast, left, op.type, right);
			break;
		case DEFINE:
			// definition name
			if(left->type != identExp) {
				new_error_token(p, IDENT);
				parser_skip_to_semi(p);
				return NULL;
			}
			char *name = left->ident.name;

			smt = new_declare_smt(p->ast, new_varible_dcl(p->ast, name, NULL, right));
	
			// Added declaration to scope
			Object *obj =(Object *)malloc(sizeof(Object));
			obj->name = name;
			obj->node = smt->declare;
			obj->type = varObj;
			parser_insert_scope(p, name, obj);
	
			break;
		default:
			// Expected an assignment operator
			new_error(p, parser_error_expect_statement, 1);
			parser_skip_to_semi(p);
	}

	// If statment is null, the next tokens dont start a valid statement
	if(smt == NULL) {
		new_error(p, parser_error_expect_statement, 1);
		parser_skip_to_semi(p);
	}

	// Release the converted expression back into the pool
	pool_release(p->ast->exp_pool, exp);
	
	return smt;
}

Smt *parse_statement_from_string(char *src) {
	parser *p = new_parser(Lex(src));
    return parse_statement(p);
}

Smt *parse_block_smt(parser *p) {
	parser_expect(p, LBRACE);
	parser_enter_scope(p);

	// build list of statements
	int smtCount = 0;
	Smt *smts = (Smt *)malloc(sizeof(Smt) * 1024);
	Smt *smtsPrt = smts;
	while(p->tokens->type != RBRACE) {
		smtCount++;
		memcpy(smtsPrt, parse_statement(p), sizeof(Smt));
		if(p->tokens->type != RBRACE) parser_expect_semi(p);
		smtsPrt++;
	}
	smts = realloc(smts, sizeof(Smt) * smtCount);

	parser_expect(p, RBRACE);
	parser_exit_scope(p);

	Smt *s = new_block_smt(p->ast, smts, smtCount);
	return s;
}

// smtd parser the current token in the context of the start of a statement
Smt *smtd(parser *p, Token *token) {
	switch(token->type) {
		// return statement
		case RETURN: {
			p->tokens++;
			Smt *s = new_ret_smt(p->ast, parse_expression(p, 0));
			return s; 
		}
		// block statement
		case LBRACE:
			return parse_block_smt(p);
		
		// if statement
		case IF: {
			p->tokens++;
			
			Exp *cond = parse_expression(p, 0);
			Smt *block = parse_block_smt(p);
			Smt *elses = NULL;

			// Check for elseif/else
			if (p->tokens->type == ELSE) {
				p->tokens++;
				if (p->tokens->type == IF) {
					// else if, so recursivly parse else chain
					elses = parse_statement(p);
				} else {
					// final else statment only has a body
					elses = new_if_smt(p->ast, NULL, parse_statement(p), NULL);
				}
			}

			return new_if_smt(p->ast, cond, block, elses);
		}
		// for loop
		case FOR: {
			p->tokens++;

			// parse index
			Dcl *index = parse_variable_dcl(p);
			parser_expect_semi(p);

			// parse condition
			Exp *cond = parse_expression(p, 0);
			parser_expect_semi(p);

			// parse increment
			Smt *inc = parse_statement(p);
			
			// parse body
			Smt *body = parse_block_smt(p);

			return new_for_smt(p->ast, index, cond, inc, body);
		}
		// varible declaration
		case VAR: {
			return new_declare_smt(p->ast, parse_variable_dcl(p));
		}
		// increment expression
		case IDENT: {
			Exp *ident = parse_ident_exp(p);

			Token one_token = {INT, 0, 0, "1"};
			Exp *one_literal = new_literal_exp(p->ast, one_token);

			switch(p->tokens->type) {
				case INC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, ADD_ASSIGN, one_literal);
				case DEC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, SUB_ASSIGN, one_literal);
				default:
					// expression is assigment or declaration so let caller handle it
					p->tokens--; // go back to ident
					return NULL;
			}
		}
		default:
			return NULL;
	}
	return NULL;
}

// Parses the next expression by binding tokens until the left binding power is 
// <= right binding power (rbp)
Exp *parse_expression(parser *p, int rbp) {
	Exp *left;
	Token *t = p->tokens;
	parser_next(p);
	left = nud(p, t);
	if(left == NULL) return NULL;
	while (rbp < get_binding_power(p->tokens->type)) {
		t = p->tokens;
		parser_next(p);
		left = led(p, t, left);
		if(left == NULL) return NULL;
	}
	return left;
}

Exp *parse_expression_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_expression(p, 0); 
}

// nud parses the current token in a prefix context (at the start of an (sub)expression)
Exp *nud(parser *p, Token *token) {
	switch (token->type) {
		case IDENT:
			return parse_ident_exp_from_token(p, token);
		
		case INT:
		case FLOAT:
		case HEX:
		case OCTAL:
		case STRING:
			return new_literal_exp(p->ast, *token);

		case NOT:
		case SUB:
			return new_unary_exp(p->ast, *token, parse_expression(p, 60));

		case LBRACE:
			return parse_key_value_list_exp(p);

		case LBRACK:
			return parse_array_exp(p);

		default: {
			// Expected a prefix token
			new_error(p, parser_error_expect_prefix, 1);
			return NULL;
		}
	}

	return NULL;
}

// led parses the current token in a infix contect (between two nodes)
Exp *led(parser *p, Token *token, Exp *exp) {
	int bp = get_binding_power(token->type);
	
	switch (token->type) {
		// binary expression
		case ADD:
		case SUB:
		case MUL:
		case QUO:
		case REM:
		case EQL:
		case NEQ:
		case GTR:
		case LSS:
		case GEQ:
		case LEQ: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp));
		}

		// selector expression
		case PERIOD: {
			return new_selector_exp(p->ast, exp, parse_expression(p, bp));
		}

		// index expression
		case LBRACK: {
			Exp *index = parse_expression(p, 0);
			parser_expect(p, RBRACK);
			
			return new_index_exp(p->ast, exp, index);
		}

		// array/struct expression
		case LBRACE: {
			printf("LBrace!");
		}

		// call expression
		case LPAREN: {
			int argCount = 0;
			Exp *args = (Exp *)malloc(0);
			if(p->tokens->type != RPAREN) {
				// arguments are not empty so parse arguments
				while(true) {
					argCount++;
					
					args = realloc(args, argCount * sizeof(Exp));
					Exp *arg = parse_expression(p, 0);
					memcpy(args + argCount - 1, arg, sizeof(Exp));
					
					if(p->tokens->type == RPAREN) break;
					parser_expect(p, COMMA);
				}
			}
			parser_expect(p, RPAREN);

			return new_call_exp(p->ast, exp, args, argCount);
		}

		// right associative binary expression or assignments
		// if the expression is an assigment, return a binary statement and let parse_statement 
		// transform it into a statement
		case LAND:
		case LOR:
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN: 
		case DEFINE: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp - 1));	
		}
		default: {
			// expected an infix expression
			new_error(p, parser_error_expect_infix, 1);
			return NULL;
		}
	}

	return NULL;
}

// Parses key value expressions in the form "expression:expression" or "expression"
Exp *parse_key_value_exp(parser *p) {
	Exp *keyOrVal = parse_expression(p, 0);
	Exp *key = NULL;
	Exp *value = NULL;
	
	if (p->tokens->type == COLON) {
		// Key/value belongs to structure expression
		p->tokens++;
		key = keyOrVal;
		value = parse_expression(p, 0);
	} else {
		// Key/value belongs to array expression
		value = keyOrVal;
	}

	return new_key_value_exp(p->ast, key, value);
}

Exp *parse_key_value_list_exp(parser *p) {
	int keyCount = 0;
	Exp *values = malloc(0);

	while(p->tokens->type != RBRACE) {
		keyCount++;
		values = realloc(values, keyCount * sizeof(Exp));
		Exp *keyValue = parse_key_value_exp(p);
		memcpy(values + keyCount - 1, keyValue, sizeof(Exp));
		
		if(p->tokens->type != RBRACE) parser_expect(p, COMMA);
	}

	return new_key_value_list_exp(p->ast, values, keyCount);
}

Exp *parse_array_exp(parser *p) {
	int valueCount = 0;
	Exp *values = malloc(0);
	while(p->tokens->type != RBRACK) {
		values = realloc(values, (++valueCount) * sizeof(Exp));
		Exp *value = parse_expression(p, 0);
		memcpy(values + valueCount - 1, value, sizeof(Exp));
		if (p->tokens->type != RBRACK) parser_expect(p, COMMA);
	}

	parser_expect(p, RBRACK);

	return new_array_exp(p->ast, values, valueCount);
}

// parse_type parses a type, adds an error if no type was found
Exp *parse_type(parser *p) {
	Exp *ident = parse_ident_exp(p);
	if (ident == NULL) {
		queue_pop_back(p->error_queue); // discard ident error
		new_error(p, parser_error_expect_type, 1);
		return NULL;
	}

	if(p->tokens->type == LBRACK) {
		// Type is an array type
		p->tokens++;
		Exp *length = parse_expression(p, 0);
		if(length == NULL) {
			new_error(p, parser_error_expect_array_length, 1);
			return NULL;
		}

		parser_expect(p, RBRACK);
		return new_array_type_exp(p->ast, ident, length);
	}

	return ident;
}

Exp *parse_ident_exp_from_token(parser *p, Token *token) {
	if(token->type != IDENT){
		// Add error to queue
		new_error_token(p, IDENT);
		return NULL;
	}

	char *name = token->value;
	Exp *ident = new_ident_exp(p->ast, name);
	Object *obj = parser_find_scope(p, name);
	ident->ident.obj = obj;
	
	return ident;
}

Exp *parse_ident_exp(parser *p) {
	Exp *ident = parse_ident_exp_from_token(p, p->tokens);
	parser_next(p);
	return ident;
}

```

#### includes/pool.h
```c
#pragma once

struct pool_element;
typedef struct pool_element pool_element;

struct pool_element {
    pool_element *next;
};

typedef struct {
    void *pool_memory;
    size_t element_size;
    int element_count;

    pool_element *head;
    pool_element *tail;
} pool;

pool *new_pool(size_t element_size, int element_count);
bool pool_full(pool *p);
int pool_size(pool *p);
int pool_count(pool *p);
void pool_extend(pool *p, int new_count);
void *pool_get(pool *p);
void pool_release(pool *p, void *element);
void pool_destroy(pool *p);
```

#### pool.c
```c
#include "includes/pool.h"

// new_pool creates a new pool
pool *new_pool(size_t element_size, int element_count) {
    // allocate space for elements + free list
    element_size = element_size + sizeof(pool_element);
    void *pool_memory = malloc(element_size * element_count);
    
    // set up the free list
    pool_element *last_element = pool_memory;
    for(int i = 0; i < element_count; i++) {
        pool_element *element = pool_memory + i * element_size;
        last_element->next = element;
        last_element = element;
    }
    last_element->next = NULL;
    
    // construct the pool data
    pool *p = malloc(sizeof(pool));
    p->pool_memory = pool_memory; 
    p->element_size = element_size;
    p->element_count = element_count;
    p->head = pool_memory;
    p->tail = last_element;
    
    return p;
}

// pool_full returns true if the pool is full
bool pool_full(pool *p) {
    return p->head == NULL;
}

// pool_size returns the size (amount of elements that can be allocated) of the pool
int pool_size(pool *p) {
    return p->element_count;
}

// pool_count returns the amount of elements in the pool
int pool_count(pool *p) {
    // if the pool is full no need to look at free list
    if(pool_full(p)) return p->element_count; 

    // count the amount of elements in the free list
    int free_count = 0;
    pool_element *ptr = p->head;
    while(ptr != p->tail){
        free_count++;
        ptr = ptr->next;
    }
    free_count++;

    return p->element_count - free_count;
}

// pool_extend extends the size of the pool
void pool_extend(pool *p, int new_count) {
    assert(new_count > p->element_count);
    int old_count = p->element_count;
    p->element_count = new_count;

    bool is_full = pool_full(p);

    // get the head/tail offset
    void *old_memory = p->pool_memory;
    int head_offset = (char *)p->head - (char *)p->pool_memory;
    int tail_offset = (char *)p->tail - (char *)p->pool_memory;
    
    // extend the pool memory
    p->pool_memory = realloc(p->pool_memory, p->element_size * p->element_count);
    assert(p->pool_memory != NULL);
    
    // restore the head/tail
    p->head = (pool_element *)((char *)p->pool_memory + head_offset);
    p->tail = (pool_element *)((char *)p->pool_memory + tail_offset);
    
    // restore the pointers in the free list
    if(!is_full) {
        pool_element *ptr = p->head;
        while(ptr != p->tail) {
            ptr->next = p->pool_memory + ((void *)ptr->next - old_memory);
            ptr = ptr->next;
        }
    }

    // get the first new element
    pool_element *first_new = p->pool_memory + (p->element_size * old_count);
    
    // set up a free list for the new elements
    pool_element *last_element = first_new;
    for(int i = old_count; i < p->element_count; i++) {
        pool_element *element = p->pool_memory + i * p->element_size;
        last_element->next = element;
        last_element = element;
    }
    
    if(is_full) {
        // set the head to the new free list
        p->head = first_new;
    } else {
        // join the old free list and the new free list
        p->tail->next = first_new;
    }

    p->tail = last_element;
}

// pool_get gets a new element from the pool, increasing the pools size if its full.
void *pool_get(pool *p) {
    if (pool_full(p)) pool_extend(p, p->element_count * 2);
    pool_element *element = p->head;
    p->head = p->head->next;
    return element + 1;
}

// pool_release releases element back into pool to be reused
void pool_release(pool *p, void *element) {
    // Check element is within bounds
    assert(element > p->pool_memory);
    assert(element < p->pool_memory + p->element_count * p->element_size);
    
    // Move pointer back to free list data
    pool_element *list_element = element;
    list_element--;

    // Add to free list
    if (pool_full(p)) {
        // Free list is empty so start a new free list
        p->head = list_element;
        p->head->next = NULL;
        p->tail = p->head;
    } else {
        // Append to free list
        p->tail->next = list_element;
        p->tail = list_element;
    }
}

// pool_destroy frees the pools memory
void pool_destroy(pool *p) {
    free(p->pool_memory);
}

```

#### includes/queue.h
```c
#pragma once

#include "all.h"

struct queue_item {
    struct queue_item *prev;
    struct queue_item *next;
};

typedef struct queue_item queue_item;

typedef struct {
    void *memory;
    size_t element_size;
    int size;
    queue_item *first;
    queue_item *last;
} queue;

queue *new_queue(size_t element_size);
int queue_size(queue *q);
void *queue_push_front(queue *q);
void *queue_push_back(queue *q);
void *queue_pop_front(queue *q);
void *queue_pop_back(queue *q);
void queue_free_item(void *item);
void queue_destroy(queue *q);
```

#### queue.c
```c
#include "includes/queue.h"

queue *new_queue(size_t element_size) {
    queue *q = malloc(sizeof(queue));
    element_size += sizeof(queue_item);
    q->element_size = element_size;
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    return q;
}

int queue_size(queue *q) {
    return q->size;
}

void *queue_push_front(queue *q) {
    q->size++;
    queue_item *q_item = malloc(q->element_size);
    
    q_item->prev = NULL;
    if (q->first == NULL) {
        q_item->next = NULL;
        q->last = q_item;
    } else {
        q_item->next = q->first;
        q->first->prev = q_item;
    }
    q->first = q_item;

    return q_item + 1; // move past item header
}

void *queue_push_back(queue *q) {
    q->size++;
    queue_item *q_item = malloc(q->element_size);
    
    q_item->next = NULL;
    if(q->first == NULL) {
        q_item->prev = NULL;
        q->first = q_item;
    } else {
        q_item->prev = q->last;
        q->last->next = q_item;
    }
    q->last = q_item;
    
    return q_item + 1; // move past item header
}

void *queue_pop_front(queue *q) {
    if(q->first == NULL) return NULL;
    q->size--;
    queue_item *q_item = q->first;
    if (q->first == q->last) q->last = NULL;
    q->first->prev = NULL;
    q->first = q->first->next;
    return q_item + 1; // move past item header
}

void *queue_pop_back(queue *q) {
    if(q->last == NULL) return NULL;
    q->size--;
    queue_item *q_item = q->last;
    if (q->last == q->first) q->first = NULL;
    q->last->next = NULL;
    q->last = q->last->prev;
    return q_item + 1; // move past item header
}

void queue_free_item(void *item) {
    queue_item *q_item = item;
    free(q_item - 1);
}

void queue_destroy(queue *q) {
    while(q->first != NULL) {
        queue_item *next = q->first->next;
        free(q->first);
        q->first = next;
    }
}

```

#### includes/string.h
```c
#pragma once

typedef struct {
    int length;
    int capacity;
} string_header;

typedef char *string;

string string_new(const char *str);
string string_new_length(const char *str, int len);
string string_new_file(FILE *f);

void string_free(string s);

string string_copy(string s);

int string_length(string s);
int string_capacity(string s);
int string_avalible(string s);

string string_expand(string s, int capacity);

string string_clear(string s);

string string_append(string s1, string s2);
string string_append_length(string s1, char *s2, int length);
string string_append_cstring(string s, char *str);

string string_slice(string s, int start, int end);

bool string_equals(string s1, string s2);
```

#### string.c
```c
#include "includes/string.h"

#define STRING_HEADER(s) ((string_header *)s - 1)

string string_new(const char *str) {
    int length = str ? strlen(str) : 0;
    return string_new_length(str, length);
}

string string_new_length(const char *str, int len) {
    // Create header + data
    void *data = malloc(sizeof(string_header) + len + 1);
    if (data == NULL) return NULL;
    
    string s = (char *)data + sizeof(string_header);
    
    // Set the header data
    string_header *header = STRING_HEADER(s);
    header->length = len;
    header->capacity = len;

    // Set the string data
    memcpy(s, str, len);
    s[len] = '\0';

    return s;
}

string string_new_file(FILE *f) {
    fseek (f, 0, SEEK_END);
	int file_length = ftell (f);
	fseek (f, 0, SEEK_SET);

    string s = string_new_length("", file_length);
    fread(s, 1, file_length, f);
    s[file_length] = '\0';
    STRING_HEADER(s)->length = file_length;

    return s;
}

void string_free(string s) {
    free(STRING_HEADER(s));
}

string string_copy(string s) {
    return string_new_length(s, string_length(s));
}

int string_length(string s) {
    return STRING_HEADER(s)->length;
}

int string_capacity(string s) {
    return STRING_HEADER(s)->capacity;
}

int string_avalible(string s) {
    string_header *header = STRING_HEADER(s);
    return header->capacity - header->length;
}

string string_expand(string s, int capacity) {
    string_header *header = STRING_HEADER(s);
    if (header->capacity > capacity) return s;
    header = realloc(header, sizeof(string_header) + capacity);
    header->capacity = capacity;
    return (char *)(header + 1);
}

string string_clear(string s) {
    return string_slice(s, 0, 0);
}

string string_append(string s1, string s2) {
    return string_append_length(s1, s2, string_length(s2));
}

string string_append_length(string s1, char *s2, int length) {
    int current_length = string_length(s1);
    s1 = string_expand(s1, current_length + length + 1);
    memcpy(s1 + current_length, s2, length);
    s1[current_length + length] = '\0';
    STRING_HEADER(s1)->length = current_length + length;
    return s1;
}

string string_append_cstring(string s, char *str) {
    return string_append_length(s, str, strlen(str));
}

string string_slice(string s, int start, int end) {
    string_header *header = STRING_HEADER(s);
    assert(start >= 0);
    assert(end <= header->length);
    int length = end - start;
    if(start > 0) memmove(s, s + start, length);
    s[length] = '\0';
    header->length = length;
    return s;
}

bool string_equals(string s1, string s2) {
    int length = string_length(s1);
    if (length != string_length(s2)) {
        return false;
    }

    for (int i = 0; i < length; i++) {
        if(s1[i] != s2[i]) return false;
    }

    return true;
}
```

#### includes/uthash.h
The only other external dependency (apart from LLVM), uthash which is a single header hash table.
```c
/*
Copyright (c) 2003-2017, Troy D. Hanson     http://troydhanson.github.com/uthash/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UTHASH_H
#define UTHASH_H

#define UTHASH_VERSION 2.0.2

#include <string.h>   /* memcmp,strlen */
#include <stddef.h>   /* ptrdiff_t */
#include <stdlib.h>   /* exit() */

/* These macros use decltype or the earlier __typeof GNU extension.
As decltype is only available in newer compilers (VS2010 or gcc 4.3+
when compiling c++ source) this code uses whatever method is needed
or, for VS2008 where neither is available, uses casting workarounds. */
#if defined(_MSC_VER)   /* MS compiler */
#if _MSC_VER >= 1600 && defined(__cplusplus)  /* VS2010 or newer in C++ mode */
#define DECLTYPE(x) (decltype(x))
#else                   /* VS2008 or older (or VS2010 in C mode) */
#define NO_DECLTYPE
#define DECLTYPE(x)
#endif
#elif defined(__BORLANDC__) || defined(__LCC__) || defined(__WATCOMC__)
#define NO_DECLTYPE
#define DECLTYPE(x)
#else                   /* GNU, Sun and other compilers */
#define DECLTYPE(x) (__typeof(x))
#endif

#ifdef NO_DECLTYPE
#define DECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  char **_da_dst = (char**)(&(dst));                                             \
  *_da_dst = (char*)(src);                                                       \
} while (0)
#else
#define DECLTYPE_ASSIGN(dst,src)                                                 \
do {                                                                             \
  (dst) = DECLTYPE(dst)(src);                                                    \
} while (0)
#endif

/* a number of the hash function use uint32_t which isn't defined on Pre VS2010 */
#if defined(_WIN32)
#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#elif defined(__WATCOMC__) || defined(__MINGW32__) || defined(__CYGWIN__)
#include <stdint.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif
#elif defined(__GNUC__) && !defined(__VXWORKS__)
#include <stdint.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif

#ifndef uthash_fatal
#define uthash_fatal(msg) exit(-1)        /* fatal error (out of memory,etc) */
#endif
#ifndef uthash_malloc
#define uthash_malloc(sz) malloc(sz)      /* malloc fcn                      */
#endif
#ifndef uthash_free
#define uthash_free(ptr,sz) free(ptr)     /* free fcn                        */
#endif
#ifndef uthash_strlen
#define uthash_strlen(s) strlen(s)
#endif
#ifndef uthash_memcmp
#define uthash_memcmp(a,b,n) memcmp(a,b,n)
#endif

#ifndef uthash_noexpand_fyi
#define uthash_noexpand_fyi(tbl)          /* can be defined to log noexpand  */
#endif
#ifndef uthash_expand_fyi
#define uthash_expand_fyi(tbl)            /* can be defined to log expands   */
#endif

/* initial number of buckets */
#define HASH_INITIAL_NUM_BUCKETS 32U     /* initial number of buckets        */
#define HASH_INITIAL_NUM_BUCKETS_LOG2 5U /* lg2 of initial number of buckets */
#define HASH_BKT_CAPACITY_THRESH 10U     /* expand when bucket count reaches */

/* calculate the element whose hash handle address is hhp */
#define ELMT_FROM_HH(tbl,hhp) ((void*)(((char*)(hhp)) - ((tbl)->hho)))
/* calculate the hash handle from element address elp */
#define HH_FROM_ELMT(tbl,elp) ((UT_hash_handle *)(((char*)(elp)) + ((tbl)->hho)))

#define HASH_VALUE(keyptr,keylen,hashv)                                          \
do {                                                                             \
  HASH_FCN(keyptr, keylen, hashv);                                               \
} while (0)

#define HASH_FIND_BYHASHVALUE(hh,head,keyptr,keylen,hashval,out)                 \
do {                                                                             \
  (out) = NULL;                                                                  \
  if (head) {                                                                    \
    unsigned _hf_bkt;                                                            \
    HASH_TO_BKT(hashval, (head)->hh.tbl->num_buckets, _hf_bkt);                  \
    if (HASH_BLOOM_TEST((head)->hh.tbl, hashval) != 0) {                         \
      HASH_FIND_IN_BKT((head)->hh.tbl, hh, (head)->hh.tbl->buckets[ _hf_bkt ], keyptr, keylen, hashval, out); \
    }                                                                            \
  }                                                                              \
} while (0)

#define HASH_FIND(hh,head,keyptr,keylen,out)                                     \
do {                                                                             \
  unsigned _hf_hashv;                                                            \
  HASH_VALUE(keyptr, keylen, _hf_hashv);                                         \
  HASH_FIND_BYHASHVALUE(hh, head, keyptr, keylen, _hf_hashv, out);               \
} while (0)

#ifdef HASH_BLOOM
#define HASH_BLOOM_BITLEN (1UL << HASH_BLOOM)
#define HASH_BLOOM_BYTELEN (HASH_BLOOM_BITLEN/8UL) + (((HASH_BLOOM_BITLEN%8UL)!=0UL) ? 1UL : 0UL)
#define HASH_BLOOM_MAKE(tbl)                                                     \
do {                                                                             \
  (tbl)->bloom_nbits = HASH_BLOOM;                                               \
  (tbl)->bloom_bv = (uint8_t*)uthash_malloc(HASH_BLOOM_BYTELEN);                 \
  if (!((tbl)->bloom_bv))  { uthash_fatal( "out of memory"); }                   \
  memset((tbl)->bloom_bv, 0, HASH_BLOOM_BYTELEN);                                \
  (tbl)->bloom_sig = HASH_BLOOM_SIGNATURE;                                       \
} while (0)

#define HASH_BLOOM_FREE(tbl)                                                     \
do {                                                                             \
  uthash_free((tbl)->bloom_bv, HASH_BLOOM_BYTELEN);                              \
} while (0)

#define HASH_BLOOM_BITSET(bv,idx) (bv[(idx)/8U] |= (1U << ((idx)%8U)))
#define HASH_BLOOM_BITTEST(bv,idx) (bv[(idx)/8U] & (1U << ((idx)%8U)))

#define HASH_BLOOM_ADD(tbl,hashv)                                                \
  HASH_BLOOM_BITSET((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1U)))

#define HASH_BLOOM_TEST(tbl,hashv)                                               \
  HASH_BLOOM_BITTEST((tbl)->bloom_bv, (hashv & (uint32_t)((1ULL << (tbl)->bloom_nbits) - 1U)))

#else
#define HASH_BLOOM_MAKE(tbl)
#define HASH_BLOOM_FREE(tbl)
#define HASH_BLOOM_ADD(tbl,hashv)
#define HASH_BLOOM_TEST(tbl,hashv) (1)
#define HASH_BLOOM_BYTELEN 0U
#endif

#define HASH_MAKE_TABLE(hh,head)                                                 \
do {                                                                             \
  (head)->hh.tbl = (UT_hash_table*)uthash_malloc(                                \
                  sizeof(UT_hash_table));                                        \
  if (!((head)->hh.tbl))  { uthash_fatal( "out of memory"); }                    \
  memset((head)->hh.tbl, 0, sizeof(UT_hash_table));                              \
  (head)->hh.tbl->tail = &((head)->hh);                                          \
  (head)->hh.tbl->num_buckets = HASH_INITIAL_NUM_BUCKETS;                        \
  (head)->hh.tbl->log2_num_buckets = HASH_INITIAL_NUM_BUCKETS_LOG2;              \
  (head)->hh.tbl->hho = (char*)(&(head)->hh) - (char*)(head);                    \
  (head)->hh.tbl->buckets = (UT_hash_bucket*)uthash_malloc(                      \
          HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));               \
  if (! (head)->hh.tbl->buckets) { uthash_fatal( "out of memory"); }             \
  memset((head)->hh.tbl->buckets, 0,                                             \
          HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));               \
  HASH_BLOOM_MAKE((head)->hh.tbl);                                               \
  (head)->hh.tbl->signature = HASH_SIGNATURE;                                    \
} while (0)

#define HASH_REPLACE_BYHASHVALUE_INORDER(hh,head,fieldname,keylen_in,hashval,add,replaced,cmpfcn) \
do {                                                                             \
  (replaced) = NULL;                                                             \
  HASH_FIND_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, replaced); \
  if (replaced) {                                                                \
     HASH_DELETE(hh, head, replaced);                                            \
  }                                                                              \
  HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, &((add)->fieldname), keylen_in, hashval, add, cmpfcn); \
} while (0)

#define HASH_REPLACE_BYHASHVALUE(hh,head,fieldname,keylen_in,hashval,add,replaced) \
do {                                                                             \
  (replaced) = NULL;                                                             \
  HASH_FIND_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, replaced); \
  if (replaced) {                                                                \
     HASH_DELETE(hh, head, replaced);                                            \
  }                                                                              \
  HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, add); \
} while (0)

#define HASH_REPLACE(hh,head,fieldname,keylen_in,add,replaced)                   \
do {                                                                             \
  unsigned _hr_hashv;                                                            \
  HASH_VALUE(&((add)->fieldname), keylen_in, _hr_hashv);                         \
  HASH_REPLACE_BYHASHVALUE(hh, head, fieldname, keylen_in, _hr_hashv, add, replaced); \
} while (0)

#define HASH_REPLACE_INORDER(hh,head,fieldname,keylen_in,add,replaced,cmpfcn)    \
do {                                                                             \
  unsigned _hr_hashv;                                                            \
  HASH_VALUE(&((add)->fieldname), keylen_in, _hr_hashv);                         \
  HASH_REPLACE_BYHASHVALUE_INORDER(hh, head, fieldname, keylen_in, _hr_hashv, add, replaced, cmpfcn); \
} while (0)

#define HASH_APPEND_LIST(hh, head, add)                                          \
do {                                                                             \
  (add)->hh.next = NULL;                                                         \
  (add)->hh.prev = ELMT_FROM_HH((head)->hh.tbl, (head)->hh.tbl->tail);           \
  (head)->hh.tbl->tail->next = (add);                                            \
  (head)->hh.tbl->tail = &((add)->hh);                                           \
} while (0)

#define HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh,head,keyptr,keylen_in,hashval,add,cmpfcn) \
do {                                                                             \
  unsigned _ha_bkt;                                                              \
  (add)->hh.hashv = (hashval);                                                   \
  (add)->hh.key = (char*) (keyptr);                                              \
  (add)->hh.keylen = (unsigned) (keylen_in);                                     \
  if (!(head)) {                                                                 \
    (add)->hh.next = NULL;                                                       \
    (add)->hh.prev = NULL;                                                       \
    (head) = (add);                                                              \
    HASH_MAKE_TABLE(hh, head);                                                   \
  } else {                                                                       \
    void *_hs_iter = (head);                                                     \
    (add)->hh.tbl = (head)->hh.tbl;                                              \
    do {                                                                         \
      if (cmpfcn(DECLTYPE(head)(_hs_iter), add) > 0)                             \
        break;                                                                   \
    } while ((_hs_iter = HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->next));         \
    if (_hs_iter) {                                                              \
      (add)->hh.next = _hs_iter;                                                 \
      if (((add)->hh.prev = HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->prev)) {     \
        HH_FROM_ELMT((head)->hh.tbl, (add)->hh.prev)->next = (add);              \
      } else {                                                                   \
        (head) = (add);                                                          \
      }                                                                          \
      HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->prev = (add);                      \
    } else {                                                                     \
      HASH_APPEND_LIST(hh, head, add);                                           \
    }                                                                            \
  }                                                                              \
  (head)->hh.tbl->num_items++;                                                   \
  HASH_TO_BKT(hashval, (head)->hh.tbl->num_buckets, _ha_bkt);                    \
  HASH_ADD_TO_BKT((head)->hh.tbl->buckets[_ha_bkt], &(add)->hh);                 \
  HASH_BLOOM_ADD((head)->hh.tbl, hashval);                                       \
  HASH_EMIT_KEY(hh, head, keyptr, keylen_in);                                    \
  HASH_FSCK(hh, head);                                                           \
} while (0)

#define HASH_ADD_KEYPTR_INORDER(hh,head,keyptr,keylen_in,add,cmpfcn)             \
do {                                                                             \
  unsigned _hs_hashv;                                                            \
  HASH_VALUE(keyptr, keylen_in, _hs_hashv);                                      \
  HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, keyptr, keylen_in, _hs_hashv, add, cmpfcn); \
} while (0)

#define HASH_ADD_BYHASHVALUE_INORDER(hh,head,fieldname,keylen_in,hashval,add,cmpfcn) \
  HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, &((add)->fieldname), keylen_in, hashval, add, cmpfcn)

#define HASH_ADD_INORDER(hh,head,fieldname,keylen_in,add,cmpfcn)                 \
  HASH_ADD_KEYPTR_INORDER(hh, head, &((add)->fieldname), keylen_in, add, cmpfcn)

#define HASH_ADD_KEYPTR_BYHASHVALUE(hh,head,keyptr,keylen_in,hashval,add)        \
do {                                                                             \
  unsigned _ha_bkt;                                                              \
  (add)->hh.hashv = (hashval);                                                   \
  (add)->hh.key = (char*) (keyptr);                                              \
  (add)->hh.keylen = (unsigned) (keylen_in);                                     \
  if (!(head)) {                                                                 \
    (add)->hh.next = NULL;                                                       \
    (add)->hh.prev = NULL;                                                       \
    (head) = (add);                                                              \
    HASH_MAKE_TABLE(hh, head);                                                   \
  } else {                                                                       \
    (add)->hh.tbl = (head)->hh.tbl;                                              \
    HASH_APPEND_LIST(hh, head, add);                                             \
  }                                                                              \
  (head)->hh.tbl->num_items++;                                                   \
  HASH_TO_BKT(hashval, (head)->hh.tbl->num_buckets, _ha_bkt);                    \
  HASH_ADD_TO_BKT((head)->hh.tbl->buckets[_ha_bkt], &(add)->hh);                 \
  HASH_BLOOM_ADD((head)->hh.tbl, hashval);                                       \
  HASH_EMIT_KEY(hh, head, keyptr, keylen_in);                                    \
  HASH_FSCK(hh, head);                                                           \
} while (0)

#define HASH_ADD_KEYPTR(hh,head,keyptr,keylen_in,add)                            \
do {                                                                             \
  unsigned _ha_hashv;                                                            \
  HASH_VALUE(keyptr, keylen_in, _ha_hashv);                                      \
  HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, keyptr, keylen_in, _ha_hashv, add);      \
} while (0)

#define HASH_ADD_BYHASHVALUE(hh,head,fieldname,keylen_in,hashval,add)            \
  HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, add)

#define HASH_ADD(hh,head,fieldname,keylen_in,add)                                \
  HASH_ADD_KEYPTR(hh, head, &((add)->fieldname), keylen_in, add)

#define HASH_TO_BKT(hashv,num_bkts,bkt)                                          \
do {                                                                             \
  bkt = ((hashv) & ((num_bkts) - 1U));                                           \
} while (0)

/* delete "delptr" from the hash table.
* "the usual" patch-up process for the app-order doubly-linked-list.
* The use of _hd_hh_del below deserves special explanation.
* These used to be expressed using (delptr) but that led to a bug
* if someone used the same symbol for the head and deletee, like
*  HASH_DELETE(hh,users,users);
* We want that to work, but by changing the head (users) below
* we were forfeiting our ability to further refer to the deletee (users)
* in the patch-up process. Solution: use scratch space to
* copy the deletee pointer, then the latter references are via that
* scratch pointer rather than through the repointed (users) symbol.
*/
#define HASH_DELETE(hh,head,delptr)                                              \
do {                                                                             \
    struct UT_hash_handle *_hd_hh_del;                                           \
    if ( ((delptr)->hh.prev == NULL) && ((delptr)->hh.next == NULL) )  {         \
        uthash_free((head)->hh.tbl->buckets,                                     \
                    (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket) ); \
        HASH_BLOOM_FREE((head)->hh.tbl);                                         \
        uthash_free((head)->hh.tbl, sizeof(UT_hash_table));                      \
        head = NULL;                                                             \
    } else {                                                                     \
        unsigned _hd_bkt;                                                        \
        _hd_hh_del = &((delptr)->hh);                                            \
        if ((delptr) == ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->tail)) {     \
            (head)->hh.tbl->tail =                                               \
                (UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) +               \
                (head)->hh.tbl->hho);                                            \
        }                                                                        \
        if ((delptr)->hh.prev != NULL) {                                         \
            ((UT_hash_handle*)((ptrdiff_t)((delptr)->hh.prev) +                  \
                    (head)->hh.tbl->hho))->next = (delptr)->hh.next;             \
        } else {                                                                 \
            DECLTYPE_ASSIGN(head,(delptr)->hh.next);                             \
        }                                                                        \
        if (_hd_hh_del->next != NULL) {                                          \
            ((UT_hash_handle*)((ptrdiff_t)_hd_hh_del->next +                     \
                    (head)->hh.tbl->hho))->prev =                                \
                    _hd_hh_del->prev;                                            \
        }                                                                        \
        HASH_TO_BKT( _hd_hh_del->hashv, (head)->hh.tbl->num_buckets, _hd_bkt);   \
        HASH_DEL_IN_BKT(hh,(head)->hh.tbl->buckets[_hd_bkt], _hd_hh_del);        \
        (head)->hh.tbl->num_items--;                                             \
    }                                                                            \
    HASH_FSCK(hh,head);                                                          \
} while (0)


/* convenience forms of HASH_FIND/HASH_ADD/HASH_DEL */
#define HASH_FIND_STR(head,findstr,out)                                          \
    HASH_FIND(hh,head,findstr,(unsigned)uthash_strlen(findstr),out)
#define HASH_ADD_STR(head,strfield,add)                                          \
    HASH_ADD(hh,head,strfield[0],(unsigned)uthash_strlen(add->strfield),add)
#define HASH_REPLACE_STR(head,strfield,add,replaced)                             \
    HASH_REPLACE(hh,head,strfield[0],(unsigned)uthash_strlen(add->strfield),add,replaced)
#define HASH_FIND_INT(head,findint,out)                                          \
    HASH_FIND(hh,head,findint,sizeof(int),out)
#define HASH_ADD_INT(head,intfield,add)                                          \
    HASH_ADD(hh,head,intfield,sizeof(int),add)
#define HASH_REPLACE_INT(head,intfield,add,replaced)                             \
    HASH_REPLACE(hh,head,intfield,sizeof(int),add,replaced)
#define HASH_FIND_PTR(head,findptr,out)                                          \
    HASH_FIND(hh,head,findptr,sizeof(void *),out)
#define HASH_ADD_PTR(head,ptrfield,add)                                          \
    HASH_ADD(hh,head,ptrfield,sizeof(void *),add)
#define HASH_REPLACE_PTR(head,ptrfield,add,replaced)                             \
    HASH_REPLACE(hh,head,ptrfield,sizeof(void *),add,replaced)
#define HASH_DEL(head,delptr)                                                    \
    HASH_DELETE(hh,head,delptr)

/* HASH_FSCK checks hash integrity on every add/delete when HASH_DEBUG is defined.
* This is for uthash developer only; it compiles away if HASH_DEBUG isn't defined.
*/
#ifdef HASH_DEBUG
#define HASH_OOPS(...) do { fprintf(stderr,__VA_ARGS__); exit(-1); } while (0)
#define HASH_FSCK(hh,head)                                                       \
do {                                                                             \
    struct UT_hash_handle *_thh;                                                 \
    if (head) {                                                                  \
        unsigned _bkt_i;                                                         \
        unsigned _count;                                                         \
        char *_prev;                                                             \
        _count = 0;                                                              \
        for( _bkt_i = 0; _bkt_i < (head)->hh.tbl->num_buckets; _bkt_i++) {       \
            unsigned _bkt_count = 0;                                             \
            _thh = (head)->hh.tbl->buckets[_bkt_i].hh_head;                      \
            _prev = NULL;                                                        \
            while (_thh) {                                                       \
               if (_prev != (char*)(_thh->hh_prev)) {                            \
                   HASH_OOPS("invalid hh_prev %p, actual %p\n",                  \
                    _thh->hh_prev, _prev );                                      \
               }                                                                 \
               _bkt_count++;                                                     \
               _prev = (char*)(_thh);                                            \
               _thh = _thh->hh_next;                                             \
            }                                                                    \
            _count += _bkt_count;                                                \
            if ((head)->hh.tbl->buckets[_bkt_i].count !=  _bkt_count) {          \
               HASH_OOPS("invalid bucket count %u, actual %u\n",                 \
                (head)->hh.tbl->buckets[_bkt_i].count, _bkt_count);              \
            }                                                                    \
        }                                                                        \
        if (_count != (head)->hh.tbl->num_items) {                               \
            HASH_OOPS("invalid hh item count %u, actual %u\n",                   \
                (head)->hh.tbl->num_items, _count );                             \
        }                                                                        \
        /* traverse hh in app order; check next/prev integrity, count */         \
        _count = 0;                                                              \
        _prev = NULL;                                                            \
        _thh =  &(head)->hh;                                                     \
        while (_thh) {                                                           \
           _count++;                                                             \
           if (_prev !=(char*)(_thh->prev)) {                                    \
              HASH_OOPS("invalid prev %p, actual %p\n",                          \
                    _thh->prev, _prev );                                         \
           }                                                                     \
           _prev = (char*)ELMT_FROM_HH((head)->hh.tbl, _thh);                    \
           _thh = ( _thh->next ?  (UT_hash_handle*)((char*)(_thh->next) +        \
                                  (head)->hh.tbl->hho) : NULL );                 \
        }                                                                        \
        if (_count != (head)->hh.tbl->num_items) {                               \
            HASH_OOPS("invalid app item count %u, actual %u\n",                  \
                (head)->hh.tbl->num_items, _count );                             \
        }                                                                        \
    }                                                                            \
} while (0)
#else
#define HASH_FSCK(hh,head)
#endif

/* When compiled with -DHASH_EMIT_KEYS, length-prefixed keys are emitted to
* the descriptor to which this macro is defined for tuning the hash function.
* The app can #include <unistd.h> to get the prototype for write(2). */
#ifdef HASH_EMIT_KEYS
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)                                   \
do {                                                                             \
    unsigned _klen = fieldlen;                                                   \
    write(HASH_EMIT_KEYS, &_klen, sizeof(_klen));                                \
    write(HASH_EMIT_KEYS, keyptr, (unsigned long)fieldlen);                      \
} while (0)
#else
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)
#endif

/* default to Jenkin's hash unless overridden e.g. DHASH_FUNCTION=HASH_SAX */
#ifdef HASH_FUNCTION
#define HASH_FCN HASH_FUNCTION
#else
#define HASH_FCN HASH_JEN
#endif

/* The Bernstein hash function, used in Perl prior to v5.6. Note (x<<5+x)=x*33. */
#define HASH_BER(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned _hb_keylen=(unsigned)keylen;                                          \
  const unsigned char *_hb_key=(const unsigned char*)(key);                      \
  (hashv) = 0;                                                                   \
  while (_hb_keylen-- != 0U) {                                                   \
      (hashv) = (((hashv) << 5) + (hashv)) + *_hb_key++;                         \
  }                                                                              \
} while (0)


/* SAX/FNV/OAT/JEN hash functions are macro variants of those listed at
* http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx */
#define HASH_SAX(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned _sx_i;                                                                \
  const unsigned char *_hs_key=(const unsigned char*)(key);                      \
  hashv = 0;                                                                     \
  for(_sx_i=0; _sx_i < keylen; _sx_i++) {                                        \
      hashv ^= (hashv << 5) + (hashv >> 2) + _hs_key[_sx_i];                     \
  }                                                                              \
} while (0)
/* FNV-1a variation */
#define HASH_FNV(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned _fn_i;                                                                \
  const unsigned char *_hf_key=(const unsigned char*)(key);                      \
  hashv = 2166136261U;                                                           \
  for(_fn_i=0; _fn_i < keylen; _fn_i++) {                                        \
      hashv = hashv ^ _hf_key[_fn_i];                                            \
      hashv = hashv * 16777619U;                                                 \
  }                                                                              \
} while (0)

#define HASH_OAT(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned _ho_i;                                                                \
  const unsigned char *_ho_key=(const unsigned char*)(key);                      \
  hashv = 0;                                                                     \
  for(_ho_i=0; _ho_i < keylen; _ho_i++) {                                        \
      hashv += _ho_key[_ho_i];                                                   \
      hashv += (hashv << 10);                                                    \
      hashv ^= (hashv >> 6);                                                     \
  }                                                                              \
  hashv += (hashv << 3);                                                         \
  hashv ^= (hashv >> 11);                                                        \
  hashv += (hashv << 15);                                                        \
} while (0)

#define HASH_JEN_MIX(a,b,c)                                                      \
do {                                                                             \
  a -= b; a -= c; a ^= ( c >> 13 );                                              \
  b -= c; b -= a; b ^= ( a << 8 );                                               \
  c -= a; c -= b; c ^= ( b >> 13 );                                              \
  a -= b; a -= c; a ^= ( c >> 12 );                                              \
  b -= c; b -= a; b ^= ( a << 16 );                                              \
  c -= a; c -= b; c ^= ( b >> 5 );                                               \
  a -= b; a -= c; a ^= ( c >> 3 );                                               \
  b -= c; b -= a; b ^= ( a << 10 );                                              \
  c -= a; c -= b; c ^= ( b >> 15 );                                              \
} while (0)

#define HASH_JEN(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned _hj_i,_hj_j,_hj_k;                                                    \
  unsigned const char *_hj_key=(unsigned const char*)(key);                      \
  hashv = 0xfeedbeefu;                                                           \
  _hj_i = _hj_j = 0x9e3779b9u;                                                   \
  _hj_k = (unsigned)(keylen);                                                    \
  while (_hj_k >= 12U) {                                                         \
    _hj_i +=    (_hj_key[0] + ( (unsigned)_hj_key[1] << 8 )                      \
        + ( (unsigned)_hj_key[2] << 16 )                                         \
        + ( (unsigned)_hj_key[3] << 24 ) );                                      \
    _hj_j +=    (_hj_key[4] + ( (unsigned)_hj_key[5] << 8 )                      \
        + ( (unsigned)_hj_key[6] << 16 )                                         \
        + ( (unsigned)_hj_key[7] << 24 ) );                                      \
    hashv += (_hj_key[8] + ( (unsigned)_hj_key[9] << 8 )                         \
        + ( (unsigned)_hj_key[10] << 16 )                                        \
        + ( (unsigned)_hj_key[11] << 24 ) );                                     \
                                                                                 \
     HASH_JEN_MIX(_hj_i, _hj_j, hashv);                                          \
                                                                                 \
     _hj_key += 12;                                                              \
     _hj_k -= 12U;                                                               \
  }                                                                              \
  hashv += (unsigned)(keylen);                                                   \
  switch ( _hj_k ) {                                                             \
     case 11: hashv += ( (unsigned)_hj_key[10] << 24 ); /* FALLTHROUGH */        \
     case 10: hashv += ( (unsigned)_hj_key[9] << 16 );  /* FALLTHROUGH */        \
     case 9:  hashv += ( (unsigned)_hj_key[8] << 8 );   /* FALLTHROUGH */        \
     case 8:  _hj_j += ( (unsigned)_hj_key[7] << 24 );  /* FALLTHROUGH */        \
     case 7:  _hj_j += ( (unsigned)_hj_key[6] << 16 );  /* FALLTHROUGH */        \
     case 6:  _hj_j += ( (unsigned)_hj_key[5] << 8 );   /* FALLTHROUGH */        \
     case 5:  _hj_j += _hj_key[4];                      /* FALLTHROUGH */        \
     case 4:  _hj_i += ( (unsigned)_hj_key[3] << 24 );  /* FALLTHROUGH */        \
     case 3:  _hj_i += ( (unsigned)_hj_key[2] << 16 );  /* FALLTHROUGH */        \
     case 2:  _hj_i += ( (unsigned)_hj_key[1] << 8 );   /* FALLTHROUGH */        \
     case 1:  _hj_i += _hj_key[0];                                               \
  }                                                                              \
  HASH_JEN_MIX(_hj_i, _hj_j, hashv);                                             \
} while (0)

/* The Paul Hsieh hash function */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__)             \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)             \
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
#define HASH_SFH(key,keylen,hashv)                                               \
do {                                                                             \
  unsigned const char *_sfh_key=(unsigned const char*)(key);                     \
  uint32_t _sfh_tmp, _sfh_len = (uint32_t)keylen;                                \
                                                                                 \
  unsigned _sfh_rem = _sfh_len & 3U;                                             \
  _sfh_len >>= 2;                                                                \
  hashv = 0xcafebabeu;                                                           \
                                                                                 \
  /* Main loop */                                                                \
  for (;_sfh_len > 0U; _sfh_len--) {                                             \
    hashv    += get16bits (_sfh_key);                                            \
    _sfh_tmp  = ((uint32_t)(get16bits (_sfh_key+2)) << 11) ^ hashv;              \
    hashv     = (hashv << 16) ^ _sfh_tmp;                                        \
    _sfh_key += 2U*sizeof (uint16_t);                                            \
    hashv    += hashv >> 11;                                                     \
  }                                                                              \
                                                                                 \
  /* Handle end cases */                                                         \
  switch (_sfh_rem) {                                                            \
    case 3: hashv += get16bits (_sfh_key);                                       \
            hashv ^= hashv << 16;                                                \
            hashv ^= (uint32_t)(_sfh_key[sizeof (uint16_t)]) << 18;              \
            hashv += hashv >> 11;                                                \
            break;                                                               \
    case 2: hashv += get16bits (_sfh_key);                                       \
            hashv ^= hashv << 11;                                                \
            hashv += hashv >> 17;                                                \
            break;                                                               \
    case 1: hashv += *_sfh_key;                                                  \
            hashv ^= hashv << 10;                                                \
            hashv += hashv >> 1;                                                 \
  }                                                                              \
                                                                                 \
    /* Force "avalanching" of final 127 bits */                                  \
    hashv ^= hashv << 3;                                                         \
    hashv += hashv >> 5;                                                         \
    hashv ^= hashv << 4;                                                         \
    hashv += hashv >> 17;                                                        \
    hashv ^= hashv << 25;                                                        \
    hashv += hashv >> 6;                                                         \
} while (0)

#ifdef HASH_USING_NO_STRICT_ALIASING
/* The MurmurHash exploits some CPU's (x86,x86_64) tolerance for unaligned reads.
* For other types of CPU's (e.g. Sparc) an unaligned read causes a bus error.
* MurmurHash uses the faster approach only on CPU's where we know it's safe.
*
* Note the preprocessor built-in defines can be emitted using:
*
*   gcc -m64 -dM -E - < /dev/null                  (on gcc)
*   cc -## a.c (where a.c is a simple test file)   (Sun Studio)
*/
#if (defined(__i386__) || defined(__x86_64__)  || defined(_M_IX86))
#define MUR_GETBLOCK(p,i) p[i]
#else /* non intel */
#define MUR_PLUS0_ALIGNED(p) (((unsigned long)p & 3UL) == 0UL)
#define MUR_PLUS1_ALIGNED(p) (((unsigned long)p & 3UL) == 1UL)
#define MUR_PLUS2_ALIGNED(p) (((unsigned long)p & 3UL) == 2UL)
#define MUR_PLUS3_ALIGNED(p) (((unsigned long)p & 3UL) == 3UL)
#define WP(p) ((uint32_t*)((unsigned long)(p) & ~3UL))
#if (defined(__BIG_ENDIAN__) || defined(SPARC) || defined(__ppc__) || defined(__ppc64__))
#define MUR_THREE_ONE(p) ((((*WP(p))&0x00ffffff) << 8) | (((*(WP(p)+1))&0xff000000) >> 24))
#define MUR_TWO_TWO(p)   ((((*WP(p))&0x0000ffff) <<16) | (((*(WP(p)+1))&0xffff0000) >> 16))
#define MUR_ONE_THREE(p) ((((*WP(p))&0x000000ff) <<24) | (((*(WP(p)+1))&0xffffff00) >>  8))
#else /* assume little endian non-intel */
#define MUR_THREE_ONE(p) ((((*WP(p))&0xffffff00) >> 8) | (((*(WP(p)+1))&0x000000ff) << 24))
#define MUR_TWO_TWO(p)   ((((*WP(p))&0xffff0000) >>16) | (((*(WP(p)+1))&0x0000ffff) << 16))
#define MUR_ONE_THREE(p) ((((*WP(p))&0xff000000) >>24) | (((*(WP(p)+1))&0x00ffffff) <<  8))
#endif
#define MUR_GETBLOCK(p,i) (MUR_PLUS0_ALIGNED(p) ? ((p)[i]) :           \
                            (MUR_PLUS1_ALIGNED(p) ? MUR_THREE_ONE(p) : \
                             (MUR_PLUS2_ALIGNED(p) ? MUR_TWO_TWO(p) :  \
                                                      MUR_ONE_THREE(p))))
#endif
#define MUR_ROTL32(x,r) (((x) << (r)) | ((x) >> (32 - (r))))
#define MUR_FMIX(_h) \
do {                 \
  _h ^= _h >> 16;    \
  _h *= 0x85ebca6bu; \
  _h ^= _h >> 13;    \
  _h *= 0xc2b2ae35u; \
  _h ^= _h >> 16;    \
} while (0)

#define HASH_MUR(key,keylen,hashv)                                     \
do {                                                                   \
  const uint8_t *_mur_data = (const uint8_t*)(key);                    \
  const int _mur_nblocks = (int)(keylen) / 4;                          \
  uint32_t _mur_h1 = 0xf88D5353u;                                      \
  uint32_t _mur_c1 = 0xcc9e2d51u;                                      \
  uint32_t _mur_c2 = 0x1b873593u;                                      \
  uint32_t _mur_k1 = 0;                                                \
  const uint8_t *_mur_tail;                                            \
  const uint32_t *_mur_blocks = (const uint32_t*)(_mur_data+(_mur_nblocks*4)); \
  int _mur_i;                                                          \
  for(_mur_i = -_mur_nblocks; _mur_i!=0; _mur_i++) {                   \
    _mur_k1 = MUR_GETBLOCK(_mur_blocks,_mur_i);                        \
    _mur_k1 *= _mur_c1;                                                \
    _mur_k1 = MUR_ROTL32(_mur_k1,15);                                  \
    _mur_k1 *= _mur_c2;                                                \
                                                                       \
    _mur_h1 ^= _mur_k1;                                                \
    _mur_h1 = MUR_ROTL32(_mur_h1,13);                                  \
    _mur_h1 = (_mur_h1*5U) + 0xe6546b64u;                              \
  }                                                                    \
  _mur_tail = (const uint8_t*)(_mur_data + (_mur_nblocks*4));          \
  _mur_k1=0;                                                           \
  switch((keylen) & 3U) {                                              \
    case 3: _mur_k1 ^= (uint32_t)_mur_tail[2] << 16; /* FALLTHROUGH */ \
    case 2: _mur_k1 ^= (uint32_t)_mur_tail[1] << 8;  /* FALLTHROUGH */ \
    case 1: _mur_k1 ^= (uint32_t)_mur_tail[0];                         \
    _mur_k1 *= _mur_c1;                                                \
    _mur_k1 = MUR_ROTL32(_mur_k1,15);                                  \
    _mur_k1 *= _mur_c2;                                                \
    _mur_h1 ^= _mur_k1;                                                \
  }                                                                    \
  _mur_h1 ^= (uint32_t)(keylen);                                       \
  MUR_FMIX(_mur_h1);                                                   \
  hashv = _mur_h1;                                                     \
} while (0)
#endif  /* HASH_USING_NO_STRICT_ALIASING */

/* iterate over items in a known bucket to find desired item */
#define HASH_FIND_IN_BKT(tbl,hh,head,keyptr,keylen_in,hashval,out)               \
do {                                                                             \
  if ((head).hh_head != NULL) {                                                  \
    DECLTYPE_ASSIGN(out, ELMT_FROM_HH(tbl, (head).hh_head));                     \
  } else {                                                                       \
    (out) = NULL;                                                                \
  }                                                                              \
  while ((out) != NULL) {                                                        \
    if ((out)->hh.hashv == (hashval) && (out)->hh.keylen == (keylen_in)) {       \
      if (uthash_memcmp((out)->hh.key, keyptr, keylen_in) == 0) {                \
        break;                                                                   \
      }                                                                          \
    }                                                                            \
    if ((out)->hh.hh_next != NULL) {                                             \
      DECLTYPE_ASSIGN(out, ELMT_FROM_HH(tbl, (out)->hh.hh_next));                \
    } else {                                                                     \
      (out) = NULL;                                                              \
    }                                                                            \
  }                                                                              \
} while (0)

/* add an item to a bucket  */
#define HASH_ADD_TO_BKT(head,addhh)                                              \
do {                                                                             \
 head.count++;                                                                   \
 (addhh)->hh_next = head.hh_head;                                                \
 (addhh)->hh_prev = NULL;                                                        \
 if (head.hh_head != NULL) { (head).hh_head->hh_prev = (addhh); }                \
 (head).hh_head=addhh;                                                           \
 if ((head.count >= ((head.expand_mult+1U) * HASH_BKT_CAPACITY_THRESH))          \
     && ((addhh)->tbl->noexpand != 1U)) {                                        \
       HASH_EXPAND_BUCKETS((addhh)->tbl);                                        \
 }                                                                               \
} while (0)

/* remove an item from a given bucket */
#define HASH_DEL_IN_BKT(hh,head,hh_del)                                          \
    (head).count--;                                                              \
    if ((head).hh_head == hh_del) {                                              \
      (head).hh_head = hh_del->hh_next;                                          \
    }                                                                            \
    if (hh_del->hh_prev) {                                                       \
        hh_del->hh_prev->hh_next = hh_del->hh_next;                              \
    }                                                                            \
    if (hh_del->hh_next) {                                                       \
        hh_del->hh_next->hh_prev = hh_del->hh_prev;                              \
    }

/* Bucket expansion has the effect of doubling the number of buckets
* and redistributing the items into the new buckets. Ideally the
* items will distribute more or less evenly into the new buckets
* (the extent to which this is true is a measure of the quality of
* the hash function as it applies to the key domain).
*
* With the items distributed into more buckets, the chain length
* (item count) in each bucket is reduced. Thus by expanding buckets
* the hash keeps a bound on the chain length. This bounded chain
* length is the essence of how a hash provides constant time lookup.
*
* The calculation of tbl->ideal_chain_maxlen below deserves some
* explanation. First, keep in mind that we're calculating the ideal
* maximum chain length based on the *new* (doubled) bucket count.
* In fractions this is just n/b (n=number of items,b=new num buckets).
* Since the ideal chain length is an integer, we want to calculate
* ceil(n/b). We don't depend on floating point arithmetic in this
* hash, so to calculate ceil(n/b) with integers we could write
*
*      ceil(n/b) = (n/b) + ((n%b)?1:0)
*
* and in fact a previous version of this hash did just that.
* But now we have improved things a bit by recognizing that b is
* always a power of two. We keep its base 2 log handy (call it lb),
* so now we can write this with a bit shift and logical AND:
*
*      ceil(n/b) = (n>>lb) + ( (n & (b-1)) ? 1:0)
*
*/
#define HASH_EXPAND_BUCKETS(tbl)                                                 \
do {                                                                             \
    unsigned _he_bkt;                                                            \
    unsigned _he_bkt_i;                                                          \
    struct UT_hash_handle *_he_thh, *_he_hh_nxt;                                 \
    UT_hash_bucket *_he_new_buckets, *_he_newbkt;                                \
    _he_new_buckets = (UT_hash_bucket*)uthash_malloc(                            \
             2UL * tbl->num_buckets * sizeof(struct UT_hash_bucket));            \
    if (!_he_new_buckets) { uthash_fatal( "out of memory"); }                    \
    memset(_he_new_buckets, 0,                                                   \
            2UL * tbl->num_buckets * sizeof(struct UT_hash_bucket));             \
    tbl->ideal_chain_maxlen =                                                    \
       (tbl->num_items >> (tbl->log2_num_buckets+1U)) +                          \
       (((tbl->num_items & ((tbl->num_buckets*2U)-1U)) != 0U) ? 1U : 0U);        \
    tbl->nonideal_items = 0;                                                     \
    for(_he_bkt_i = 0; _he_bkt_i < tbl->num_buckets; _he_bkt_i++)                \
    {                                                                            \
        _he_thh = tbl->buckets[ _he_bkt_i ].hh_head;                             \
        while (_he_thh != NULL) {                                                \
           _he_hh_nxt = _he_thh->hh_next;                                        \
           HASH_TO_BKT( _he_thh->hashv, tbl->num_buckets*2U, _he_bkt);           \
           _he_newbkt = &(_he_new_buckets[ _he_bkt ]);                           \
           if (++(_he_newbkt->count) > tbl->ideal_chain_maxlen) {                \
             tbl->nonideal_items++;                                              \
             _he_newbkt->expand_mult = _he_newbkt->count /                       \
                                        tbl->ideal_chain_maxlen;                 \
           }                                                                     \
           _he_thh->hh_prev = NULL;                                              \
           _he_thh->hh_next = _he_newbkt->hh_head;                               \
           if (_he_newbkt->hh_head != NULL) { _he_newbkt->hh_head->hh_prev =     \
                _he_thh; }                                                       \
           _he_newbkt->hh_head = _he_thh;                                        \
           _he_thh = _he_hh_nxt;                                                 \
        }                                                                        \
    }                                                                            \
    uthash_free( tbl->buckets, tbl->num_buckets*sizeof(struct UT_hash_bucket) ); \
    tbl->num_buckets *= 2U;                                                      \
    tbl->log2_num_buckets++;                                                     \
    tbl->buckets = _he_new_buckets;                                              \
    tbl->ineff_expands = (tbl->nonideal_items > (tbl->num_items >> 1)) ?         \
        (tbl->ineff_expands+1U) : 0U;                                            \
    if (tbl->ineff_expands > 1U) {                                               \
        tbl->noexpand=1;                                                         \
        uthash_noexpand_fyi(tbl);                                                \
    }                                                                            \
    uthash_expand_fyi(tbl);                                                      \
} while (0)


/* This is an adaptation of Simon Tatham's O(n log(n)) mergesort */
/* Note that HASH_SORT assumes the hash handle name to be hh.
* HASH_SRT was added to allow the hash handle name to be passed in. */
#define HASH_SORT(head,cmpfcn) HASH_SRT(hh,head,cmpfcn)
#define HASH_SRT(hh,head,cmpfcn)                                                 \
do {                                                                             \
  unsigned _hs_i;                                                                \
  unsigned _hs_looping,_hs_nmerges,_hs_insize,_hs_psize,_hs_qsize;               \
  struct UT_hash_handle *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail;            \
  if (head != NULL) {                                                            \
      _hs_insize = 1;                                                            \
      _hs_looping = 1;                                                           \
      _hs_list = &((head)->hh);                                                  \
      while (_hs_looping != 0U) {                                                \
          _hs_p = _hs_list;                                                      \
          _hs_list = NULL;                                                       \
          _hs_tail = NULL;                                                       \
          _hs_nmerges = 0;                                                       \
          while (_hs_p != NULL) {                                                \
              _hs_nmerges++;                                                     \
              _hs_q = _hs_p;                                                     \
              _hs_psize = 0;                                                     \
              for ( _hs_i = 0; _hs_i  < _hs_insize; _hs_i++ ) {                  \
                  _hs_psize++;                                                   \
                  _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?              \
                          ((void*)((char*)(_hs_q->next) +                        \
                          (head)->hh.tbl->hho)) : NULL);                         \
                  if (! (_hs_q) ) { break; }                                     \
              }                                                                  \
              _hs_qsize = _hs_insize;                                            \
              while ((_hs_psize > 0U) || ((_hs_qsize > 0U) && (_hs_q != NULL))) {\
                  if (_hs_psize == 0U) {                                         \
                      _hs_e = _hs_q;                                             \
                      _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?          \
                              ((void*)((char*)(_hs_q->next) +                    \
                              (head)->hh.tbl->hho)) : NULL);                     \
                      _hs_qsize--;                                               \
                  } else if ( (_hs_qsize == 0U) || (_hs_q == NULL) ) {           \
                      _hs_e = _hs_p;                                             \
                      if (_hs_p != NULL){                                        \
                        _hs_p = (UT_hash_handle*)((_hs_p->next != NULL) ?        \
                                ((void*)((char*)(_hs_p->next) +                  \
                                (head)->hh.tbl->hho)) : NULL);                   \
                       }                                                         \
                      _hs_psize--;                                               \
                  } else if ((                                                   \
                      cmpfcn(DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_p)), \
                             DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl,_hs_q))) \
                             ) <= 0) {                                           \
                      _hs_e = _hs_p;                                             \
                      if (_hs_p != NULL){                                        \
                        _hs_p = (UT_hash_handle*)((_hs_p->next != NULL) ?        \
                               ((void*)((char*)(_hs_p->next) +                   \
                               (head)->hh.tbl->hho)) : NULL);                    \
                       }                                                         \
                      _hs_psize--;                                               \
                  } else {                                                       \
                      _hs_e = _hs_q;                                             \
                      _hs_q = (UT_hash_handle*)((_hs_q->next != NULL) ?          \
                              ((void*)((char*)(_hs_q->next) +                    \
                              (head)->hh.tbl->hho)) : NULL);                     \
                      _hs_qsize--;                                               \
                  }                                                              \
                  if ( _hs_tail != NULL ) {                                      \
                      _hs_tail->next = ((_hs_e != NULL) ?                        \
                            ELMT_FROM_HH((head)->hh.tbl,_hs_e) : NULL);          \
                  } else {                                                       \
                      _hs_list = _hs_e;                                          \
                  }                                                              \
                  if (_hs_e != NULL) {                                           \
                  _hs_e->prev = ((_hs_tail != NULL) ?                            \
                     ELMT_FROM_HH((head)->hh.tbl,_hs_tail) : NULL);              \
                  }                                                              \
                  _hs_tail = _hs_e;                                              \
              }                                                                  \
              _hs_p = _hs_q;                                                     \
          }                                                                      \
          if (_hs_tail != NULL){                                                 \
            _hs_tail->next = NULL;                                               \
          }                                                                      \
          if ( _hs_nmerges <= 1U ) {                                             \
              _hs_looping=0;                                                     \
              (head)->hh.tbl->tail = _hs_tail;                                   \
              DECLTYPE_ASSIGN(head,ELMT_FROM_HH((head)->hh.tbl, _hs_list));      \
          }                                                                      \
          _hs_insize *= 2U;                                                      \
      }                                                                          \
      HASH_FSCK(hh,head);                                                        \
 }                                                                               \
} while (0)

/* This function selects items from one hash into another hash.
* The end result is that the selected items have dual presence
* in both hashes. There is no copy of the items made; rather
* they are added into the new hash through a secondary hash
* hash handle that must be present in the structure. */
#define HASH_SELECT(hh_dst, dst, hh_src, src, cond)                              \
do {                                                                             \
  unsigned _src_bkt, _dst_bkt;                                                   \
  void *_last_elt=NULL, *_elt;                                                   \
  UT_hash_handle *_src_hh, *_dst_hh, *_last_elt_hh=NULL;                         \
  ptrdiff_t _dst_hho = ((char*)(&(dst)->hh_dst) - (char*)(dst));                 \
  if (src != NULL) {                                                             \
    for(_src_bkt=0; _src_bkt < (src)->hh_src.tbl->num_buckets; _src_bkt++) {     \
      for(_src_hh = (src)->hh_src.tbl->buckets[_src_bkt].hh_head;                \
          _src_hh != NULL;                                                       \
          _src_hh = _src_hh->hh_next) {                                          \
          _elt = ELMT_FROM_HH((src)->hh_src.tbl, _src_hh);                       \
          if (cond(_elt)) {                                                      \
            _dst_hh = (UT_hash_handle*)(((char*)_elt) + _dst_hho);               \
            _dst_hh->key = _src_hh->key;                                         \
            _dst_hh->keylen = _src_hh->keylen;                                   \
            _dst_hh->hashv = _src_hh->hashv;                                     \
            _dst_hh->prev = _last_elt;                                           \
            _dst_hh->next = NULL;                                                \
            if (_last_elt_hh != NULL) { _last_elt_hh->next = _elt; }             \
            if (dst == NULL) {                                                   \
              DECLTYPE_ASSIGN(dst,_elt);                                         \
              HASH_MAKE_TABLE(hh_dst,dst);                                       \
            } else {                                                             \
              _dst_hh->tbl = (dst)->hh_dst.tbl;                                  \
            }                                                                    \
            HASH_TO_BKT(_dst_hh->hashv, _dst_hh->tbl->num_buckets, _dst_bkt);    \
            HASH_ADD_TO_BKT(_dst_hh->tbl->buckets[_dst_bkt],_dst_hh);            \
            (dst)->hh_dst.tbl->num_items++;                                      \
            _last_elt = _elt;                                                    \
            _last_elt_hh = _dst_hh;                                              \
          }                                                                      \
      }                                                                          \
    }                                                                            \
  }                                                                              \
  HASH_FSCK(hh_dst,dst);                                                         \
} while (0)

#define HASH_CLEAR(hh,head)                                                      \
do {                                                                             \
  if (head != NULL) {                                                            \
    uthash_free((head)->hh.tbl->buckets,                                         \
                (head)->hh.tbl->num_buckets*sizeof(struct UT_hash_bucket));      \
    HASH_BLOOM_FREE((head)->hh.tbl);                                             \
    uthash_free((head)->hh.tbl, sizeof(UT_hash_table));                          \
    (head)=NULL;                                                                 \
  }                                                                              \
} while (0)

#define HASH_OVERHEAD(hh,head)                                                   \
 ((head != NULL) ? (                                                             \
 (size_t)(((head)->hh.tbl->num_items   * sizeof(UT_hash_handle))   +             \
          ((head)->hh.tbl->num_buckets * sizeof(UT_hash_bucket))   +             \
           sizeof(UT_hash_table)                                   +             \
           (HASH_BLOOM_BYTELEN))) : 0U)

#ifdef NO_DECLTYPE
#define HASH_ITER(hh,head,el,tmp)                                                \
for(((el)=(head)), ((*(char**)(&(tmp)))=(char*)((head!=NULL)?(head)->hh.next:NULL)); \
  (el) != NULL; ((el)=(tmp)), ((*(char**)(&(tmp)))=(char*)((tmp!=NULL)?(tmp)->hh.next:NULL)))
#else
#define HASH_ITER(hh,head,el,tmp)                                                \
for(((el)=(head)), ((tmp)=DECLTYPE(el)((head!=NULL)?(head)->hh.next:NULL));      \
  (el) != NULL; ((el)=(tmp)), ((tmp)=DECLTYPE(el)((tmp!=NULL)?(tmp)->hh.next:NULL)))
#endif

/* obtain a count of items in the hash */
#define HASH_COUNT(head) HASH_CNT(hh,head)
#define HASH_CNT(hh,head) ((head != NULL)?((head)->hh.tbl->num_items):0U)

typedef struct UT_hash_bucket {
	struct UT_hash_handle *hh_head;
	unsigned count;

	/* expand_mult is normally set to 0. In this situation, the max chain length
	* threshold is enforced at its default value, HASH_BKT_CAPACITY_THRESH. (If
	* the bucket's chain exceeds this length, bucket expansion is triggered).
	* However, setting expand_mult to a non-zero value delays bucket expansion
	* (that would be triggered by additions to this particular bucket)
	* until its chain length reaches a *multiple* of HASH_BKT_CAPACITY_THRESH.
	* (The multiplier is simply expand_mult+1). The whole idea of this
	* multiplier is to reduce bucket expansions, since they are expensive, in
	* situations where we know that a particular bucket tends to be overused.
	* It is better to let its chain length grow to a longer yet-still-bounded
	* value, than to do an O(n) bucket expansion too often.
	*/
	unsigned expand_mult;

} UT_hash_bucket;

/* random signature used only to find hash tables in external analysis */
#define HASH_SIGNATURE 0xa0111fe1u
#define HASH_BLOOM_SIGNATURE 0xb12220f2u

typedef struct UT_hash_table {
	UT_hash_bucket *buckets;
	unsigned num_buckets, log2_num_buckets;
	unsigned num_items;
	struct UT_hash_handle *tail; /* tail hh in app order, for fast append    */
	ptrdiff_t hho; /* hash handle offset (byte pos of hash handle in element */

				   /* in an ideal situation (all buckets used equally), no bucket would have
				   * more than ceil(#items/#buckets) items. that's the ideal chain length. */
	unsigned ideal_chain_maxlen;

	/* nonideal_items is the number of items in the hash whose chain position
	* exceeds the ideal chain maxlen. these items pay the penalty for an uneven
	* hash distribution; reaching them in a chain traversal takes >ideal steps */
	unsigned nonideal_items;

	/* ineffective expands occur when a bucket doubling was performed, but
	* afterward, more than half the items in the hash had nonideal chain
	* positions. If this happens on two consecutive expansions we inhibit any
	* further expansion, as it's not helping; this happens when the hash
	* function isn't a good fit for the key domain. When expansion is inhibited
	* the hash will still work, albeit no longer in constant time. */
	unsigned ineff_expands, noexpand;

	uint32_t signature; /* used only to find hash tables in external analysis */
#ifdef HASH_BLOOM
	uint32_t bloom_sig; /* used only to test bloom exists in external analysis */
	uint8_t *bloom_bv;
	uint8_t bloom_nbits;
#endif

} UT_hash_table;

typedef struct UT_hash_handle {
	struct UT_hash_table *tbl;
	void *prev;                       /* prev element in app order      */
	void *next;                       /* next element in app order      */
	struct UT_hash_handle *hh_prev;   /* previous hh in bucket order    */
	struct UT_hash_handle *hh_next;   /* next hh in bucket order        */
	void *key;                        /* ptr to enclosing struct's key  */
	unsigned keylen;                  /* enclosing struct's key len     */
	unsigned hashv;                   /* result of hash-fcn(key)        */
} UT_hash_handle;

#endif /* UTHASH_H */

```

### tests/
This folder contains the C++ program that tests the compiler

#### tests/add.fur
```markdown
proc add :: int a, int b -> int {
    return a + b
}

proc main :: -> int {
    return add(120, 3)
}
```

#### tests/arrayInit.fur
```markdown
proc main :: -> int {
    a := [100, 20, 3]
    return a[0] + a[1] + a[2]
}
```

#### tests/arraySum.fur
```markdown
proc sum :: int[3] nums -> int {
    s := 0
    for i := 0; i < 3; i++ {
        s += nums[i]
    }

    return s
}

proc main :: -> int {
    a := [100, 20, 3]
    return sum(a)
}
```

#### tests/binaryFloat.fur
```markdown
proc main :: -> int {
    return 130.75 - 7.75
}
```

#### tests/binaryInt.fur
```markdown
proc main :: -> int {
    return 120 + 3
}
```

#### tests/bubbleSort.fur
```markdown
proc sort :: int[5] items -> int[5] {
    n := 5
    for i := 0; i < n-1; i++ {
        for j := 0; j < n-i-1; j++ {
            if items[j] > items[j+1] {
                temp := items[j]
                items[j] = items[j+1]
                items[j+1] = temp
            }
        }
        n = 5
    }

    return items
}

proc main :: -> int {
    a := [54, 2, 42, 5, 6]
    a = sort(a)

    if a[0] < a[1] < a[2] < a[3] < a[4] < a[5] {
        return 123
    }
    
    return 0
}
```

#### tests/fibbonanci.fur
```markdown
proc fib :: int n -> int {
    if n < 2 {
        return n
    }

    return fib(n - 1) + fib(n - 2)
}

proc main :: -> int {
    return fib(12)
}
```

#### tests/for.fur
```markdown
proc main :: -> int {
    a := 0
    for i := 0; i < 123; i++ {
        a += 1
    }
    return a
}
```

#### tests/gcd.fur
```markdown
proc gcd :: int a, int b -> int {
    if b == 0 {
        return a
    } 
    
    return gcd(b, a % b)
}

proc main :: -> int {
    return gcd(1529, 14039)
}
```

#### tests/if.fur
```markdown
proc main :: -> int {
    if true { 
        return 123 
    }
    return 321
}
```

#### tests/ifElse.fur
```markdown
proc main :: -> int {
    if true {
        return 123
    } else {
        return 321
    }
}
```

#### tests/ifElseIfElse.fur
```markdown
proc main :: -> int {
    if false {
        return 321
    } else if true {
        return 123
    } else {
        return 0
    }
}
```

#### tests/ifElseIfElseIfElse.fur
```markdown
proc main :: -> int {
    if false {
        return 321
    } else if false {
        return 23
    } else if false {
        return 21
    } else {
        return 123
    }
}
```

#### tests/literal.fur
```markdown
proc main :: -> int {
    return 123
}
```

#### tests/longVar.fur
```markdown
proc main :: -> int {
    var int a = 123
    return a
}
```

#### tests/nestedFor.fur
```markdown
proc main :: -> int {
    s := 0
    for i := 0; i < 3; i++ {
        for j := 0; j < 41; j++ {
            s += 1
        }
    }

    return s
}
```

#### tests/reassignArg.fur
```markdown
proc test :: int n -> int {
    n = 123
    return n
}

proc main :: -> int {
    return test(321)
}
```

#### tests/shortVar.fur
```markdown
proc main :: -> int {
    a := 123
    return a
}
```

#### tests/unary.fur
```markdown
proc neg :: int n -> int {
    return -n
}

proc main :: -> int {
    return neg(-123)
}
```

#### irgen_test.cpp
Contains unit tests for the IR generation and the integrations tests for the whole system
```cpp
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#define TEST_TYPE(name, src, expectedType) TEST(IrgenTest, name){       \
    parser *p = new_parser(Lex((char *)src));                           \
    Exp *e = parse_type(p);                                             \
    Irgen *irgen = NewIrgen();                                          \
    LLVMTypeRef type = CompileType(e);                                  \
    ASSERT_TRUE(type == expectedType);                                  \
}                                                                       \

TEST_TYPE(CompileTypeInt, "int", LLVMInt64Type())
TEST_TYPE(CompileTypeI64, "i64", LLVMInt64Type())
TEST_TYPE(CompileTypeI32, "i32", LLVMInt32Type())
TEST_TYPE(CompileTypeI16, "i16", LLVMInt16Type())
TEST_TYPE(CompileTypeI8, "i8", LLVMInt8Type())

TEST_TYPE(CompileTypeFloat, "float", LLVMFloatType())
TEST_TYPE(CompileTypeF64, "f64", LLVMDoubleType())
TEST_TYPE(CompileTypeF32, "f32", LLVMFloatType())

TEST_TYPE(CompileTypeIntArray, "int[3]", LLVMArrayType(LLVMInt64Type(), 3))
TEST_TYPE(CompileTypeFloatArray, "float[100]", LLVMArrayType(LLVMFloatType(), 100))

#define TEST_LITERAL(name, src, expectedType, expectedValue) TEST(IrgenTest, name) {    \
    parser *p = new_parser(Lex((char *)src));                                           \
    Exp *e = parse_expression(p, 0);                                                    \
    Irgen *irgen = NewIrgen();                                                          \
    LLVMValueRef value = CompileLiteralExp(irgen, e);                                   \
    ASSERT_TRUE(LLVMIsConstant(value));                                                 \
    ASSERT_STREQ(expectedValue, LLVMPrintValueToString(value));                         \
}                                                                                       \

TEST_LITERAL(CompileLiteralInt, "123", LLVMInt64Type(), "i64 123")
TEST_LITERAL(CompileLiteralFloat, "123.321", LLVMFloatType(), "float 0x405ED48B40000000")
TEST_LITERAL(CompileLiteralHex, "0x1000", LLVMInt64Type(), "i64 4096")
TEST_LITERAL(CompileLiteralOctal, "0123", LLVMInt64Type(), "i64 83")

#define TEST_CAST(name, value, cast) TEST(IrgenTest, name) {                        \
    Irgen *irgen = NewIrgen();                                                      \
    irgen->function = LLVMAddFunction(irgen->module, "test", LLVMInt64Type());      \
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");       \
    irgen->builder = LLVMCreateBuilder();                                           \
    LLVMPositionBuilderAtEnd(irgen->builder, entry);                                \
    LLVMValueRef castValue = Cast(irgen, value, cast);                              \
    ASSERT_EQ(LLVMTypeOf(castValue), cast);                                         \
}                                                                                   \

TEST_CAST(I64ToI32, LLVMConstIntOfString(LLVMInt64Type(), "123", 10), LLVMInt32Type())
TEST_CAST(I16ToFloat, LLVMConstIntOfString(LLVMInt16Type(), "32", 10), LLVMFloatType())
TEST_CAST(FloatToDouble, LLVMConstRealOfString(LLVMFloatType(), "1.42"), LLVMDoubleType())
TEST_CAST(DoubleToI64, LLVMConstRealOfString(LLVMDoubleType(), "1245.12"), LLVMInt64Type())
```

#### lexer_test.cpp
Unit tests for the lexer
```cpp
#include <gtest/gtest.h>

struct tcase {
    const char *input;
    TokenType expectedType;
    const char *expectedValue;
};

TEST(LexerTest, Identifier) {

    tcase cases[] = {
        tcase{"test", IDENT, "test"},
        tcase{"a", IDENT, "a" },
        tcase{"test123", IDENT, "test123"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}

TEST(LexerTest, Numbers) {
    tcase cases[] = {
        tcase{ "1", INT, "1" },
        tcase{ "1204", INT, "1204" },

        tcase{ "213.42", FLOAT, "213.42"},
        tcase{ "0.5", FLOAT, ".5" },
        
        tcase{"0x1000", HEX, "1000"},
        tcase{"0600", OCTAL, "600"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}

TEST(LexerTest, Strings) {
    tcase cases[] = {
        tcase{ "\"test\"", STRING, "test" },
        tcase{ "\"\"", STRING, "" },

        tcase{ "\"\n\"", STRING, "\n" },
        tcase{ "\"\021\"", STRING, "\021" },
        tcase{ "\"\x41\"", STRING, "\x41" },
        tcase{ "\"\u1000\"", STRING, "\u1000" },
        tcase{ "\"\u10001000\"", STRING, "\u10001000" },
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}

TEST(LexerTest, Symbols) {
    tcase cases[] = {
        tcase{ ":", COLON, "" },
        tcase{ ":=", DEFINE, "" },
        tcase{ "::", DOUBLE_COLON, "" },

        tcase{ ".", PERIOD, "" },
        tcase{ "...", ELLIPSE, "" },

        tcase{ ",", COMMA, "" },

        tcase{ "(", LPAREN, "" },
        tcase{ ")", RPAREN, "" },
        tcase{ "[", LBRACK, "" },
        tcase{ "]", RBRACK, "" },
        tcase{ "{", LBRACE, "" },
        tcase{ "}", RBRACE, "" },

        tcase{ "+", ADD, "" },
        tcase{ "+=", ADD_ASSIGN, "" },
        tcase{ "++", INC, "" },

        tcase{ "-", SUB, "" },
        tcase{ "-=", SUB_ASSIGN, "" },
        tcase{ "--", DEC, "" },
        tcase{ "->", ARROW, "" },

        tcase{ "*", MUL, "" },
        tcase{ "*=", MUL_ASSIGN, "" },

        tcase{ "/", QUO, "" },
        tcase{ "/=", QUO_ASSIGN, "" },

        tcase{ "%", REM, "" },
        tcase{ "%=", REM_ASSIGN, "" },

        tcase{ "^", XOR, "" },
        tcase{ "^=", XOR_ASSIGN, "" },

        tcase{ "<", LSS, "" },
        tcase{ "<=", LEQ, "" },
        tcase{ "<<", SHL, "" },
        tcase{ "<<=", SHL_ASSIGN, "" },

        tcase{ ">", GTR, "" },
        tcase{ ">=", GEQ, "" },
        tcase{ ">>", SHR, "" },
        tcase{ ">>=", SHR_ASSIGN, "" },

        tcase{ "=", ASSIGN, "" },
        tcase{ "==", EQL, "" },

        tcase{ "!", NOT, "" },
        tcase{ "!=", NEQ, "" },

        tcase{ "&", AND, "" },
        tcase{ "&=", AND_ASSIGN, "" },
        tcase{ "&&", LAND, "" },
        tcase{ "&^", AND_NOT, "" },
        tcase{ "&^=", AND_NOT_ASSIGN, "" },
    
        tcase{"|", OR, ""},
        tcase{"||", LOR, ""},
        tcase{"|=", OR_ASSIGN, ""},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}

TEST(LexerTest, LineNumbers) {
    Token *tokens = Lex((char *)"1\n2\n3");
    
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(i+1, tokens[i].line);	
    }
}

TEST(LexerTest, ColumnNumbers) {
    Token *tokens = Lex((char *)"foo bar baz");

    ASSERT_EQ(1, tokens[0].column);
    ASSERT_EQ(5, tokens[1].column);
    ASSERT_EQ(9, tokens[2].column);
}

TEST(LexerTest, SemiColonInsertion) {
    Token *tokens = Lex((char *)"foo\nbar");
    ASSERT_STREQ(TokenName(SEMI), TokenName(tokens[1].type));
}
```

#### parser_test.cpp
Unit tests for the parser
```cpp
TEST(ParserTest, ScopeEnter) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    ASSERT_EQ(outer->outer, NULL);
    parser_enter_scope(p);
    ASSERT_TRUE(outer == p->scope->outer);
}

TEST(ParserTest, ScopeExit) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    parser_enter_scope(p);
    parser_exit_scope(p);
    ASSERT_TRUE(outer == p->scope);
}

TEST(ParserTest, ScopeInsert) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = new_argument_dcl(p->ast, NULL, (char *)"test_name");
    bool inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_TRUE(inserted);

    scope_object *found;
    HASH_FIND_STR((scope_object *)p->scope->objects, (char *)"test", found);
    ASSERT_STREQ(obj->name, found->obj->name);
    ASSERT_STREQ(obj->node->argument.name, 
        (char *)found->obj->node->argument.name);			

    inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_FALSE(inserted);
}

TEST(ParserTest, ScopeFind) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = NULL;
    parser_insert_scope(p, (char *)"test", obj);

    // Enter and exit some scopes
    parser_enter_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);

    Object *found = parser_find_scope(p, (char *)"test");
    ASSERT_EQ(obj->name, found->name); // pointer should be same

    found = parser_find_scope(p, (char *)"not here");
    ASSERT_EQ(found, NULL);
}

TEST(ParserTest, ParseLiteralExpression) {
    Exp *exp = parse_expression_from_string((char *)"123");

    ASSERT_FALSE(exp == NULL);
    ASSERT_EQ((int)literalExp, (int)exp->type);
    ASSERT_STREQ("123", exp->literal.value);
}

TEST(ParserTest, ParseIdentExpression) {
    Exp *exp = parse_expression_from_string((char *)"test");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test", exp->ident.name);
}

TEST(ParserTest, ParseIdentExpressionWithNumber) {
    Exp *exp = parse_expression_from_string((char *)"test123");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test123", exp->ident.name);
}

TEST(ParserTest, ParseBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
}

TEST(ParserTest, ParseBidmasBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b * c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)ADD, (int)exp->binary.op.type);
    ASSERT_EQ((int)MUL, (int)exp->binary.right->binary.op.type);
}

TEST(ParserTest, ParseSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.selector->ident.name);
}

TEST(ParserTest, ParseDoubleSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b.c");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_EQ((int)selectorExp, (int)exp->selector.exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.exp->selector.selector->ident.name);
    ASSERT_STREQ("c", exp->selector.selector->ident.name);
}

TEST(ParserTest, ParseIndexExpression) {
    Exp *exp = parse_expression_from_string((char *)"test[1]");

    ASSERT_EQ((int)indexExp, (int)exp->type);
}

TEST(ParserTest, ParseRightAssociativeBinaryOperators) {
    Exp *exp = parse_expression_from_string((char *)"a || b || c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)binaryExp, (int)exp->binary.right->type);
}

TEST(ParserTest, ParseUnaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"!a");
    
    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseUnaryMinusExpression) {
    Exp *exp = parse_expression_from_string((char *)"-a");

    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseAssignmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a = b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}

TEST(ParserTest, ParseAddAssigmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a += b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
    ASSERT_EQ((int)binaryExp, (int)smt->assignment.right->type);
    ASSERT_EQ((int)ADD, (int)smt->assignment.right->binary.op.type);
    ASSERT_STREQ((char *)"a", smt->assignment.right->binary.left->ident.name);
    ASSERT_STREQ((char *)"b", smt->assignment.right->binary.right->ident.name);
}

TEST(ParserTest, ParseReturnStatment) {
    Smt *smt = parse_statement_from_string((char *)"return a");

    ASSERT_EQ((int)returnSmt, (int)smt->type);
}

TEST(ParserTest, ParseBlockStatment) {
    Smt *smt = parse_statement_from_string((char *)"{\nreturn test\n}");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);
}

TEST(ParserTest, ParserBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);    
}

TEST(ParserTest, ParserLongBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ a = 1; b = 2; return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(3, smt->block.count);
}

TEST(ParserTest, ParseIfStatment) {
    Smt *smt = parse_statement_from_string((char *)"if true {\nreturn false\n}");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_EQ(smt->ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseStatement) {
    Smt *smt = parse_statement_from_string((char *)"if true { return 123 } else { return 321 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elses = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseIfElseStatment) {
    Smt *smt = parse_statement_from_string((char *)"if false { return 321 } else if true { return 123 } else { return 0 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elif = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elif->type);
    ASSERT_NE(elif->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elif->ifs.body->type);
    ASSERT_NE(elif->ifs.elses, NULL);

    Smt *elses = elif->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}

TEST(ParserTest, ParserShortVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"a := 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_TRUE(obj->node == smt->declare);
}

TEST(ParserTest, ParseLongVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"var int a = 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);
    ASSERT_STREQ("a", smt->declare->varible.name);
    ASSERT_EQ((int)identExp, (int)smt->declare->varible.type->type);
    ASSERT_STREQ("int", smt->declare->varible.type->ident.name);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_NE(obj, NULL);
    ASSERT_TRUE(obj->node == smt->declare);
}

TEST(ParserTest, ParseArrayType) {
    const char *src = "int[3]";
    parser *p = new_parser(Lex((char *)src));
    Exp *type = parse_type(p);

    ASSERT_EQ((int)arrayTypeExp, (int)type->type);
}

TEST(ParserTest, ParseFunctionDefinition) {
    const char *src = "proc test :: int a, int b -> int {\nreturn a + b\n}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ((int)functionDcl, (int)dcl->type);
    ASSERT_EQ(2, (int)dcl->function.argCount);
    ASSERT_EQ((int)identExp, (int)dcl->function.returnType->type);

    Object *obja = parser_find_scope(p, (char *)"a");
    Object *objb = parser_find_scope(p, (char *)"b");
    
    ASSERT_NE(obja, NULL);
    ASSERT_NE(objb, NULL);
    ASSERT_NE(obja->node, objb->node);
    ASSERT_EQ(dcl->function.args, obja->node);
    ASSERT_EQ(dcl->function.args + 1, objb->node);
}

TEST(ParserTest, ParseEmptyCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test()");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(0, exp->call.argCount);
}

TEST(ParserTest, ParseCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(1, test)");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(2, exp->call.argCount);

    ASSERT_STREQ("1", exp->call.args[0].literal.value);
}

TEST(ParserTest, ParseCallInCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(test())");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(1, exp->call.argCount);
}

TEST(ParserTest, ParseForLoop) {
    Smt *smt = parse_statement_from_string((char *)"for i := 0; i < 10; i += 1 {}");

    ASSERT_EQ((int)forSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->fors.index->type);
    ASSERT_EQ((int)binaryExp, (int)smt->fors.cond->type);
    ASSERT_EQ((int)assignmentSmt, (int)smt->fors.inc->type);
    ASSERT_EQ((int)blockSmt, (int)smt->fors.body->type);
}

TEST(ParserTest, ParseIncrement) {
    Smt *smt = parse_statement_from_string((char *)"i++");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}

TEST(ParserTest, ParseKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{a: 1, b: 2}");

    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(2, exp->keyValueList.keyCount);
    ASSERT_STREQ("a", exp->keyValueList.keyValues[0].keyValue.key->ident.name);
    ASSERT_STREQ("b", exp->keyValueList.keyValues[1].keyValue.key->ident.name);
}

TEST(ParserTest, ParseEmptyKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(0, exp->keyValueList.keyCount);
}

TEST(ParserTest, ParseNullKeyValueList) {   
    Exp *exp = parse_expression_from_string((char *)"{1, 2, 3}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(3, exp->keyValueList.keyCount);
}

TEST(ParserTest, ParseArrayExpression) {
    Exp *exp = parse_expression_from_string((char *)"[1, 2, 3]");

    ASSERT_EQ((int)arrayExp, (int)exp->type);
    ASSERT_EQ(3, exp->array.valueCount);
}

TEST(ParserTest, ParseFunctionDclWithoutProc) {
    parser *p = new_parser(Lex((char *)"add :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(PROC, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutName) {
    parser *p = new_parser(Lex((char *)"proc :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutArgumentSeperator) {
    parser *p = new_parser(Lex((char *)"proc add -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(DOUBLE_COLON, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutCommas) {
    parser *p = new_parser(Lex((char *)"proc add :: int a int b int c -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(2, queue_size(p->error_queue));
    
    for (int i = 0; i < 2; i++) {
        parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
        ASSERT_EQ(parser_error_expect_token, error->type);
        ASSERT_EQ(1, error->length);
        ASSERT_EQ(COMMA, error->expect_token.type);
    }
}

TEST(ParserTest, ParseFunctionDclWithoutArgTypeOrName) {
    parser *p = new_parser(Lex((char *)"proc add :: int, int a -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutArrow) {
    parser *p = new_parser(Lex((char *)"proc add :: int a {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ARROW, error->expect_token.type);
}

TEST(ParserTest, ParseNonDecleration) {
    parser *p = new_parser(Lex((char *)"return a"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_declaration, error->type);
    ASSERT_EQ(1, error->length);
}

TEST(ParserTest, ParseVaribleDclWithoutType) {
    parser *p = new_parser(Lex((char *)"var = 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_type, error->type);
    ASSERT_EQ(1, error->length);
}

TEST(ParserTest, ParseVariableDclWithoutName) {
    parser *p = new_parser(Lex((char *)"var int = 100"));
    Dcl *dcl = parse_declaration(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}

TEST(ParserTest, ParseVaribleDclWithoutEquals) {
    parser *p = new_parser(Lex((char *)"var int a 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ASSIGN, error->expect_token.type);
}

TEST(ParserTest, ParseVaribleDclWithoutValue) {
    parser *p = new_parser(Lex((char *)"var int a = "));
    Dcl *dcl = parse_declaration(p);
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_prefix, error->type);
    ASSERT_EQ(1, error->length);
}

```

#### pool_test.cpp
Unit tests for the pool
```cpp
#include <gtest/gtest.h>

TEST(PoolTest, NewPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}

TEST(PoolTest, InsertPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int v1 = 123;
    int *e1 = (int *)pool_get(int_pool);
    memcpy(e1, &v1, sizeof(int));

    ASSERT_EQ(v1, *e1);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(1, pool_count(int_pool));
    pool_destroy(int_pool);
}

TEST(PoolTest, FillPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 5; i++) {
        int *e = (int *)pool_get(int_pool);
        memcpy(e, &i, sizeof(int));
    }

    ASSERT_TRUE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(5, pool_count(int_pool));
    pool_destroy(int_pool);
}

TEST(PoolTest, ExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_extend(int_pool, 10);
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}

TEST(PoolTest, OverflowExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 6; i++) {
        int *pool_element = (int *)pool_get(int_pool);
        memcpy(pool_element, &i, sizeof(int));
    }

    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(6, pool_count(int_pool));
    pool_destroy(int_pool);
}

TEST(PoolTest, ReleaseElementFromPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));
    
    ASSERT_EQ(3, pool_count(int_pool));
    
    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));
    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);

    pool_destroy(int_pool);
}

TEST(PoolTest, ReusePool) {
    pool *int_pool = new_pool(sizeof(int), 3);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));

    ASSERT_EQ(3, pool_count(int_pool));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(3, *e3);

    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));

    int *e4 = (int *)pool_get(int_pool);
    ASSERT_EQ(*e3, *e4);
    
    int v4 = 4;
    memcpy(e4, &v4, sizeof(int));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(4, *e4);
}
```

#### queue_test.cpp
Unit tests for the queue
```cpp
#include <gtest/gtest.h>

TEST(QueueTest, NewQueue) {
    queue *q = new_queue(sizeof(int));
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(sizeof(int) + sizeof(queue_item), q->element_size);
}

TEST(QueueTest, PushPopFront) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_front(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_front(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}

TEST(QueueTest, PushPopBack) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_back(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_back(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}

TEST(QueueTest, PushPopManyFront) {
    queue *q = new_queue(sizeof(int));
    for(int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_back(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 0; i < 100; i++) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}

TEST(QueueTest, PushPopManyBack) {
    queue *q = new_queue(sizeof(int));
    for (int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_front(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 99; i >= 0; i--) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
```

#### string_test.cpp
Unit tests for the string
```cpp
#define STRING_HEADER(s) ((string_header *)s - 1)

TEST(StringTest, CreateNewString) {
    string s = string_new("test");
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}

TEST(StringTest, CreateNewStringLength) {
    string s = string_new_length("testtesttest", 4);
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}

TEST(StringTest, CreateNewStringFile) {
    FILE *f = fopen("/tmp/string_test_file.fur", "w");
    fprintf(f, "test");
    fclose(f);

    f = fopen("/tmp/string_test_file.fur", "r");
    string s = string_new_file(f);
    fclose(f);

    ASSERT_EQ(0, strcmp(s, "test"));
}

TEST(StringTest, CopyString) {
    string s = string_new("test");
    string copy = string_copy(s);
    ASSERT_NE(s, copy);
    ASSERT_EQ(0, strcmp(s, copy));
}

TEST(StringTest, StringLength) {
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}

TEST(StringTest, StringCapacity) { 
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}

TEST(StringTest, StringAvalible) {
    string s = string_new("test");
    ASSERT_EQ(0, string_avalible(s));
}

TEST(StringTest, StringExpand) {
    string s = string_new("test");
    s = string_expand(s, 10);
    ASSERT_EQ(10, string_capacity(s));
    ASSERT_EQ(6, string_avalible(s));
    ASSERT_EQ(4, string_length(s));
    ASSERT_EQ(0, strcmp(s, "test"));
}

TEST(StringTest, StringClear) {
    string s = string_new("test");
    s = string_clear(s);
    ASSERT_EQ(0, strcmp(s, ""));
}

TEST(StringTest, StringAppend){
    string s1 = string_new("hello ");
    string s2 = string_new("world");
    s1 = string_append(s1, s2);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringAppendLength) {
    string s1 = string_new("hello ");
    string s2 = string_new("world of earth");
    s1 = string_append_length(s1, s2, 5);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringAppendCString) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringMultipleAppendWithMalloc) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    malloc(10);
    s1 = string_append_cstring(s1, (char *)" of");
    s1 = string_append_cstring(s1, (char *)" ours");

    ASSERT_EQ(0, strcmp(s1, "hello world of ours"));
}

TEST(StringTest, StringSliceEnd) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 0, 11);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}

TEST(StringTest, StringSlice) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 6, 11);
    ASSERT_EQ(0, strcmp(s1, "world"));
}

TEST(StringTest, StringEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar");
    ASSERT_EQ(true, string_equals(s1, s2));
}

TEST(StringTest, StringNotEqualsLength) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}

TEST(StringTest, StringNotEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}
```

#### test.cpp
Test program entry point
```cpp
#include <gtest/gtest.h>

#include <stdio.h>
#include <stdarg.h>

// src project
extern "C" {
    #include "../src/includes/error.h"
    #include "../src/includes/lexer.h"
    #include "../src/includes/ast.h"
    #include "../src/includes/parser.h"
    #include "../src/includes/irgen.h"
    #include "../src/includes/pool.h"
    #include "../src/includes/queue.h"
    #include "../src/includes/string.h"
}

// test files
#include "pool_test.cpp"
#include "queue_test.cpp"
#include "string_test.cpp"
#include "lexer_test.cpp"
#include "parser_test.cpp"
#include "irgen_test.cpp"
#include "integration_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    // ::testing::GTEST_FLAG(filter) = "*Pool";
    return RUN_ALL_TESTS();
}

```