#include "fur.h"

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
	return 0;
}

void ParserNext(Parser *parser) {
	parser->tokens++;
}

void expect(Parser *parser, TokenType type) {
	// TODO: create TokenType -> string
	ASSERT(parser->tokens->type == type, "Expect failed");
	ParserNext(parser);
}

void expectSemi(Parser *parser) {
	ASSERT(parser->tokens->type == SEMI || 
		parser->tokens->type == END, "Expected semi");
	ParserNext(parser);
}

// nud parses the current token in a prefix context (at the start of an (sub)expression)
Exp *nud(Parser *parser, Token *token) {
	switch (token->type) {
	case IDENT:
		return newIdentExp(token->value);
	case NOT:
	case SUB:
		return newUnaryExp(*token, ParseExpression(parser, 60));
	}

	return NULL;
}

// led parses the current token in a infix contect (between two nodes)
Exp *led(Parser *parser, Token *token, Exp *exp) {
	int bp = BindingPower(token->type);
	
	switch (token->type) {
		// binary expression
		case ADD:
		case SUB:
		case MUL:
		case QUO:
		case EQL:
		case NEQ:
		case GTR:
		case LSS:
		case GEQ:
		case LEQ: {
			return newBinaryExp(exp, *token, ParseExpression(parser, bp));
		} 
		// selector expression
		case PERIOD: {
			return newSelectorExp(exp, ParseExpression(parser, bp));
		}
		// index expression
		case LBRACK: {
			Exp *index = ParseExpression(parser, 0);
			expect(parser, RBRACK);
			return newIndexExp(exp, index);
		}
		// right associative binary expression or assignments
		// if the expression is an assigment, return a binary statement and let
		// ParseStatment transform it into a statment.
		case LAND:
		case LOR:
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN: {
			return newBinaryExp(exp, *token, ParseExpression(parser, bp - 1));	
		}
	}

	return NULL;
}

Smt *smtd(Parser *parser, Token *token) {
	switch(token->type) {
		case RETURN: {
			parser->tokens++;
			Smt *s = newReturnSmt(ParseExpression(parser, 0));
			expectSemi(parser);
			return s; 
		}
	}
	return NULL;
}

// Parses the next statement by calling smtd on the first token else handle
// the declaration/assignment
Smt *ParseStatment(Parser *parser) {
	Token *t = parser->tokens;
	Smt *smt = smtd(parser, t);
	if (smt != NULL) {
		return smt;
	}

	// Statement is an assignment/declaration, so treat it like an expression
	// and transform it.
	Exp *exp = ParseExpression(parser, 0);
	ASSERT(exp->type == binaryExp, "Expecting assigment/declation statement");
	
	Exp *left = exp->node.binary.left;
	Exp *right = exp->node.binary.right;

	switch(exp->node.binary.op.type) {
		case ASSIGN:
			smt = newAssignmentSmt(left, right);
			break;
		case ADD_ASSIGN:
			smt = newAddAssignmentSmt(left, right);
			break;
		case SUB_ASSIGN:
			smt = newSubAssignmentSmt(left, right);
			break;
		case MUL_ASSIGN:
			smt = newMulAssignmentSmt(left, right);
			break;
		case REM_ASSIGN:
			smt = newRemAssignmentSmt(left, right);
			break;
		case OR_ASSIGN:
			smt = newOrAssignmentSmt(left, right);
			break;
		case SHL_ASSIGN:
			smt = newShlAssignmentSmt(left, right);
			break;
	}

	// If statment is null, the next tokens dont start a valid statement
	ASSERT(smt != NULL, "Expecting assigment/declation statement");

	free(exp);
	return smt;
}

// Parses the next expression by binding tokens until the left binding power is 
// <= right binding power (rbp)
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