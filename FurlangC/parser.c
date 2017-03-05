#include "parser.h"

Parser *NewParser(Token *tokens) {
	Scope *scope = (Scope *)malloc(sizeof(Scope));
	scope->outer = NULL;
	scope->objects = NULL;
	
	Parser *parser = (Parser *)malloc(sizeof(Parser));
	parser->tokens = tokens;
	parser->expLevel = 0;
	parser->rhs = false;
	
	return parser;
}

void EnterScope(Parser *parser) {
	Scope *scope = (Scope *)malloc(sizeof(Scope));
	scope->outer = parser->scope->outer;
	scope->objects = NULL;

	parser->scope = scope;
}

void ExitScope(Parser *parser) {
	// clear hash table and free all scope objects
	ScopeObject *obj, *tmp;
	HASH_ITER(hh, parser->scope->objects, obj, tmp) {
		HASH_DEL(parser->scope->objects, obj);
		free(obj);
	}

	// Move to outer scipe
	Scope *outer = parser->scope->outer;
	free(parser->scope);
	parser->scope = outer;
}

void InsertScope(Parser *parser, char *name, Object *object) {
	// check if name is already in scope
	ScopeObject *obj;
	HASH_FIND_STR(parser->scope->objects, name, obj);
	if (obj != NULL) printf("\"%s\" already in scope", name);

	// add object to scope
	obj = (ScopeObject *)malloc(sizeof(ScopeObject));
	obj->name = name;
	obj->obj = object;
	HASH_ADD_KEYPTR(hh, parser->scope->objects, obj->name, strlen(obj->name), obj);
}

Object *FindScope(Parser *parser, char *name) {
	ScopeObject *obj;
	for (Scope *scope = parser->scope; scope != NULL; scope = scope->outer) {
		HASH_FIND_STR(scope->objects, name, obj);
		if (obj != NULL) return obj->obj;
	}

	printf("\"%s\" not in scope", name);
}

int BindingPower(TokenType type) {
	switch (type) {
	// Non-binding operators
	case SEMI:
		return 0;
	// Assignment operators
	case ASSIGN:
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
		return 50;
	// Special unary
	case NOT:
		return 60;
	// Strongly bound
	case PERIOD:
	case LBRACK:
	case LPAREN:
		return 70;
	}
}

void ParserNext(Parser *parser) {
	parser->tokens++;
}

void expect(Parser *parser, TokenType type) {
	ParserNext(parser);
	if (parser->tokens->type != type)
		printf("Expected: %d, Got: %d", type, parser->tokens->type);
}

Exp *nud(Parser *parser, Token *token) {
	switch (token->type) {
	case IDENT:
		return newIdentExp(token->value);
	}
}

Exp *led(Parser *parser, Token *token, Exp *exp) {
	switch (token->type) {
	case ADD:
	case SUB:
	case MUL:
	case QUO:
	case EQL:
	case NEQ:
	case GTR:
	case LSS:
	case GEQ:
	case LEQ:
		return newBinaryExp(exp, *token, ParseExpression(parser, BindingPower(token->type)));
	/*case PERIOD:
		if (exp->type != IDENT) {
			printf("Expected identifier on the left hand side of '.'");
		}*/
	case LBRACK:
		return NULL;
	}
}

// Binds tokens until the left binding power is <= right binding power (rbp)
Exp *ParseExpression(Parser *parser, int rbp) {
	Exp *left;
	Token *t = parser->tokens;
	ParserNext(parser);
	left = nud(parser, t);
	while (rbp < BindingPower(parser->tokens->type)) {
		t = parser->tokens;
		ParserNext(parser);
		left = led(parser, t, left);
	}
	return left;
}