#pragma once

#include "all.h"
#include "uthash.h"

typedef struct {
	char *name;
	Object *obj;
	UT_hash_handle hh;
} ScopeObject;

struct _Scope {
	struct _Scope *outer;
	ScopeObject *objects;
};

typedef struct _Scope Scope;

typedef struct {
	char *src;
	Scope *scope;
    Token *tokens;
    int expLevel; // >= 0 when inside expression
    bool rhs; // true if parser is parsing right hand side
} Parser;

Parser *NewParser(char *src, Token *tokens);
Exp *ParseIdentToken(Parser *parser, Token *token);
Exp *ParseExpression(Parser *parser, int rbp);
Smt *ParseStatement(Parser *parser);