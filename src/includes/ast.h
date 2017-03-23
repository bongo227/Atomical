#pragma once

#include "all.h"
#include <llvm-c/Core.h>

struct _Exp;
typedef struct _Exp Exp;

struct _Dcl;
typedef struct _Dcl Dcl;

struct _Smt;
typedef struct _Smt Smt;

typedef struct {
	Dcl **dcls;
	int dclCount;
} File;

typedef enum {
	badObj,
	pkgObj,
	constObj,
	typeObj,
	varObj,
	argObj,
	funcObj,
} ObjectType;

// Object 
// An object represents a named construct such as a constant, type, varible, function.
typedef struct {
	ObjectType type;
	char *name;
	Dcl *node;
} Object;

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
	procTypeExp, // TODO: add this
} ExpType;

struct _Exp {
	ExpType type;
	union {
		// identExp
		struct {
			char *name;
			Object *obj;
		} ident;

		// literalExp
		Token literal;

		// selectorExp
		struct {
			Exp *exp;
			Exp *selector;
		} selector;

		// indexExp
		struct {
			Exp *exp;
			Exp *index;
		} index;

		// sliceExp
		struct {
			Exp *exp;
			Exp *low;
			Exp *high;
		} slice;

		// callExp
		struct {
			Exp *function;
			Exp *args;
			int argCount;
		} call;

		// starExp
		Exp *star;

		// unaryExp
		struct {
			Token op;
			Exp *right;
		} unary;

		// binaryExp
		struct {
			Exp *left;
			Token op;
			Exp *right;
		} binary;

		// keyValueExp
		struct {
			Exp *key;
			Exp *value;
		} keyValue;

		// keyValueListExp
		struct {
			Exp *keyValues;
			int keyCount;
		} keyValueList;

		// structValueExp
		struct {
			Exp *type; // array or struct type
			Exp *list; // keyValueListExp
		} structValue;

		// arrayExp
		struct {
			Exp *values;
			int valueCount;
		} array;

		// arrayTypeExp
		struct {
			Exp *type;
			Exp *length;
		} arrayType;

		// fieldTypeExp
		struct {
			Exp *type;
			Exp *name;
		} fieldType;

		// structTypeExp
		struct {
			Exp *fields;
			int feildCount; 
		} structType;
	} node;
};

Exp *newIdentExp(char *ident);
Exp *newUnaryExp(Token op, Exp *right);
Exp *newBinaryExp(Exp *left, Token op, Exp *right);
Exp *newSelectorExp(Exp *exp, Exp* selector);
Exp *newIndexExp(Exp *exp, Exp *index);
Exp *newArrayTypeExp(Exp *type, Exp *length);

typedef enum {
	functionDcl,
	argumentDcl,
	varibleDcl,
} DclType;

struct _Dcl {
	DclType type;
	LLVMValueRef llvmValue;
	union {
		// functionDcl
		struct {
			char *name;
			struct _Dcl *args;
			int argCount;
			Exp *returnType;
			struct _Smt *body;
		} function;

		// argumentDcl
		struct {
			Exp *type;
			char *name; 
		} argument;

		// varibleDcl
		struct {
			char *name;
			Exp *type;
			Exp *value;
		} varible;
	} node;
};

typedef struct _Dcl Dcl;

Dcl *newArgumentDcl(Exp *type, char *name);
Dcl *newVaribleDcl(char *name, Exp *type, Exp *value);
Dcl *newFunctionDcl(char *name, Dcl *args, int argCount, Exp *returnType, Smt *body);

typedef enum {
	declareSmt,
	assignmentSmt,
	returnSmt,
	blockSmt,
	ifSmt,
	forSmt,
} SmtType;

struct _Smt {
	SmtType type;
	union {
		// declareSmt
		Dcl *declare;
		
		// assignmentSmt
		struct {
			Exp *left;
			Exp *right;
		} assignment;

		// returnSmt
		struct {
			Exp *result;
		} ret;

		// blockSmt
		struct {
			Smt *smts;
			int count;
		} block;

		// ifSmt
		struct {
			Exp *cond;
			Smt *body;
			Smt *elses;
		} ifs;

		// forSmt
		struct {
			Dcl *index;
			Exp *cond;
			Smt *inc;
			Smt *body;
		} fors;
	} node;
};

typedef struct _Smt Smt;

Smt *newReturnSmt(Exp *result);
Smt *newBlockSmt(Smt *smts, int smtCount);
Smt *newIfSmt(Exp *cond, Smt *body, Smt *elses);
Smt *newDeclareSmt(Dcl *dcl);
Smt *newBinaryAssignmentSmt(Exp *left, TokenType op, Exp *right);