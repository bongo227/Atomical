#pragma once

# include "lexer.h"

typedef enum {
	badObj,
	pkgObj,
	constObj,
	typeObj,
	varObj,
	funcObj,
} ObjectType;

// Object 
// An object represents a named construct such as a constant, type, varible, function.
typedef struct {
	ObjectType type;
	char *name;
	void *node;
	void *typeInfo;
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

		// parenExp
		struct _Exp *paren;

		// selectorExp
		struct {
			struct _Exp *exp;
			struct _Exp *selector;
		} selector;

		// braceLiteralExp
		// TODO: add type
		struct {
			Token lBrace;
			struct _Exp *exp;
			int expCount;
			Token rBrace;
		} braceLiteral;

		// parenLiteralExp
		struct {
			Token lParen;
			struct _Exp *exp;
			int expCount;
			Token rParen;
		} parenLiteral;

		// indexExp
		struct {
			struct _Exp *exp;
			struct _Exp *index;
		} index;

		// sliceExp
		struct {
			struct _Exp *exp;
			Token lBrack;
			struct _Exp *low;
			Token Colon;
			struct _Exp *high;
			Token rBrack;
		} slice;

		// callExp
		struct {
			struct _Exp *function;
			struct _Exp *args;
		} call;

		// castExp
		struct {
			Token lParen;
			struct _Exp *exp;
			Token rParen;
		} castExp;

		// starExp
		struct _Exp *star;

		// unaryExp
		struct {
			Token op;
			struct _Exp *right;
		} unary;

		// binaryExp
		struct {
			struct _Exp *left;
			Token op;
			struct _Exp *right;
		} binary;

		// arrayExp
		struct {
			struct _Exp *type;
			struct _Exp *length;
		} arrayType;

		// fieldExp
		struct {
			struct _Exp *type;
			struct _Exp *name;
		} fieldType;

		// structExp
		struct {
			struct _Exp *fields;
			int feildCount; 
		} structType;
	} node;
};

typedef struct _Exp Exp;

typedef enum {
	functionDcl,
	argumentDcl,
	varibleDcl,
} DclType;

struct _Dcl {
	DclType type;
	union {
		// functionDcl
		struct {
			Exp *name;
			Token dColon;
			struct _Dcl *args;
			int argCount;
			struct _Smt *body;
		} function;

		// argumentDcl
		struct {
			Exp *name;
		} argument;

		// varibleDcl
		struct {
			Exp *name;
			Exp *type;
			Exp *value;
		} varible;
	} node;
};

typedef struct _Dcl Dcl;

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
			struct _Smt *smts;
			int count;
		} block;

		// ifSmt
		struct {
			Exp *cond;
			struct _Smt *body;
			struct _Smt *elses;
		} ifs;
	} node;
};

typedef struct _Smt Smt;

// TODO: finish these and consider name scheme
Exp *newIdentExp(char *ident);
Exp *newBinaryExp(Exp *left, Token op, Exp *right);
Exp *newSelectorExp(Exp *exp, Exp* selector);