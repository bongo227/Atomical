#pragma once

# include "lexer.h"

typedef struct {
	scope outer;
	
} scope;

// Object 
// An object represents a named construct such as a constant, type, varible, function.
typedef struct {
	enum {
		badObj,
		pkgObj,
		constObj,
		typeObj,
		varObj,
		funcObj,
	} type;
	char *name;
	void *node;
	void *type;
} Object;

typedef struct {
	enum { 
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
		unaryExp 
	} type;
	union {
		// identExp
		struct {
			char *name;
			Object *obj;
		} ident;

		// literalExp
		Token literal;

		// parenExp
		Exp *paren;

		struct {
			Exp *exp;
			Exp *selector;
		} selector;

		// braceLiteralExp
		// TODO: add type
		struct {
			Token lBrace;
			Exp *exp;
			int expCount;
			Token rBrace;
		} braceLiteral;

		// parenLiteralExp
		struct {
			Token lParen;
			Exp *exp;
			int expCount;
			Token rParen;
		} parenLiteral;

		// indexExp
		struct {
			Exp *exp;
			Token lBrack;
			Exp *index;
			Token rBrack;
		} index;

		// sliceExp
		struct {
			Exp *exp;
			Token lBrack;
			Exp *low;
			Token Colon;
			Exp *high;
			Token rBrack;
		} slice;

		// callExp
		struct {
			Exp *function;
			Exp *args;
		} call;

		// castExp
		struct {
			Token lParen;
			Exp *exp;
			Token rParen;
		} castExp;

		// starExp
		Exp *star;

		// unaryExp
		struct {
			Token Op;
			Exp *right;
		} unary;

		// binaryExp
		struct {
			Exp *left;
			Token Op;
			Exp *right;
		} binary;
	} node;
} Exp;

typedef struct {
	enum {
		functionDcl,
		argumentDcl,
		varibleDcl,
	} type;
	union {
		// functionDcl
		struct {
			Exp *name;
			Token dColon;
			Dcl *args;
			int argCount;
			Smt *body;
		} function;

		// argumentDcl
		struct {
			Exp *name;
		} argument;

		// varibleDcl
		struct {
			Exp *name;
			Exp *value;
		} varible;
	} node;
} Dcl;

typedef struct {
	enum {
		declareSmt,
		assignmentSmt,
		returnSmt,
		blockSmt,
		ifSmt,
		forSmt,
	} type;
	union {
		// declareSmt
		Dcl *declare;
		
		// assignmentSmt
		struct {
			Exp *left;
			Token assign;
			Exp *right;
		} assignment;

		// returnSmt
		struct {
			Token ret;
			Exp *result;
		} ret;

		// blockSmt
		struct {
			Token lBrace;
			Smt *smts;
			int smtCount;
			Token rBrace;
		} block;

		// ifSmt
		struct {
			Token ifs;
			Exp *cond;
			Smt *body;
			Smt *elses;
		} ifs;
	} node;
} Smt;

typedef struct {
	Dcl *functions;
	int functionCount;
} Ast;