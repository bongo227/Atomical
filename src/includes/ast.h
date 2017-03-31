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
