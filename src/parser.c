#include "includes/uthash.h"
#include "includes/parser.h"

// creates a new empty scope
Scope *newScope(Scope *outer) {
	Scope *scope = (Scope *)malloc(sizeof(Scope));
	scope->outer = outer;
	ScopeObject *objects = NULL;
	scope->objects = objects;

	return scope;
}

// NewParser creates a new parser
Parser *NewParser(Token *tokens) {
	Parser *parser = (Parser *)malloc(sizeof(Parser));
	parser->tokens = tokens;
	parser->scope = newScope(NULL);
	parser->expLevel = 0;
	parser->rhs = false;
	
	return parser;
}

// EnterScope enters a new inner scope
void EnterScope(Parser *parser) {
	parser->scope = newScope(parser->scope);
}

// ExitScope exits the current scope
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

// InsertScope inserts an object into the current scope
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

// FindScope finds an object in scope
Object *FindScope(Parser *parser, char *name) {
	ScopeObject *obj;
	for (Scope *scope = parser->scope; scope != NULL; scope = scope->outer) {
		HASH_FIND_STR(scope->objects, name, obj);
		if (obj != NULL) return obj->obj;
	}

	printf("\"%s\" not in scope", name);
	return NULL;
}

// BindingPower returns the left binding power of a token
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

// ParserNext moves the parser onto the next token
void ParserNext(Parser *parser) {
	parser->tokens++;
}

// expect asserts that the token is of type type, if true parser advances
void expect(Parser *parser, TokenType type) {
	// TODO: create TokenType -> string
	ASSERT(parser->tokens->type == type, "Expect failed");
	ParserNext(parser);
}

// expectSemi expects a semicolon
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
	
	case INT:
	case FLOAT:
	case HEX:
	case OCTAL:
	case STRING:
		return newLiteralExp(*token);

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
		case SHL_ASSIGN: 
		case DEFINE: {
			return newBinaryExp(exp, *token, ParseExpression(parser, bp - 1));	
		}
	}

	return NULL;
}

Exp *ParseType(Parser *parser) {
	Exp *type = ParseExpression(parser, 0);
	switch (type->type) {
	case identExp:
		break;
	case indexExp:
		// type is array type so convert indexExp -> arrayTypeExp
		type = newArrayTypeExp(type->node.index.exp,
			type->node.index.index);
		break;
	default:
		ASSERT(false, "Expecting a type");
	}

	return type;
}

// smtd parser the current token in the context of the start of a statement
Smt *smtd(Parser *parser, Token *token) {
	switch(token->type) {
		// return statement
		case RETURN: {
			parser->tokens++;
			Smt *s = newReturnSmt(ParseExpression(parser, 0));
			expectSemi(parser);
			return s; 
		}
		// block statement
		case LBRACE: {
			parser->tokens++;
			
			EnterScope(parser);

			int smtCount = 0;
			Smt *smts = (Smt *)malloc(sizeof(Smt) * 1024);
			Smt *smtsPrt = smts;
			while(parser->tokens->type != RBRACE) {
				smtCount++;
				memcpy(smtsPrt, ParseStatement(parser), sizeof(Smt));
				smtsPrt++;
			}
			smts = realloc(smts, sizeof(Smt) * smtCount);

			Smt *s = newBlockSmt(smts, smtCount);

			expect(parser, RBRACE);

			ExitScope(parser);

			return s;
		}
		// if statement
		case IF: {
			parser->tokens++;
			
			Exp *cond = ParseExpression(parser, 0);
			Smt *block = ParseStatement(parser);
			ASSERT(block->type == blockSmt, "Expected block after if condition");
			Smt *elses = NULL;

			// Check for elseif/else
			if (parser->tokens->type == ELSE) {
				parser->tokens++;
				if (parser->tokens->type == IF) {
					// else if, so recursivly parse else chain
					elses = ParseStatement(parser);
				}

				// final else statment only has a body
				elses = newIfSmt(NULL, ParseStatement(parser), NULL);
			}

			return newIfSmt(cond, block, elses);
		}
		// varible declaration
		case VAR: {
			parser->tokens++;
			
			// parse declaration type
			Exp *type = ParseType(parser);

			// parse declaration name
			ASSERT(parser->tokens->type == IDENT,
				"Expected var to have name after type");
			Exp *name = newIdentExp(parser->tokens->value);
			ParserNext(parser);

			expect(parser, ASSIGN);

			// parse declaration value
			Exp *value = ParseExpression(parser, 0);

			Smt *smt = newDeclareSmt(newVaribleDcl(name, type, value));

			// Added declaration to scope
			Object *obj =(Object *)malloc(sizeof(Object));
			obj->name = name->node.ident.name;
			obj->node = smt->node.declare;
			obj->type = varObj;
			obj->typeInfo = NULL;
			InsertScope(parser, name->node.ident.name, obj);

			return smt;
		}
	}
	return NULL;
}

Exp *ParseIdent(Parser *parser) {
	ASSERT(parser->tokens->type == IDENT,
		"Expected identifier");
	Exp *ident = newIdentExp(parser->tokens->value);
	ParserNext(parser);

	return ident;
}

Dcl *ParseFunction(Parser *parser) {
	expect(parser, PROC);
	Exp *name = ParseIdent(parser); // function name
	expect(parser, DOUBLE_COLON);

	// parse arguments
	Dcl *args = (Dcl *)malloc(0);
	int argCount = 0;
	while(parser->tokens->type != ARROW) {
		if (argCount > 0) expect(parser, COMMA);
		args = realloc(args, sizeof(Dcl) * ++argCount);

		// Construct argument
		Exp *type = ParseType(parser); // arg type
		Exp *name = ParseIdent(parser); // arg name
		Dcl *arg = newArgumentDcl(type, name);
		memcpy(args + argCount - 1, arg, sizeof(Dcl));
	}

	expect(parser, ARROW);

	Exp *returnType = ParseType(parser);
	Smt *body = ParseStatement(parser);

	return newFunctionDcl(name, args, argCount, returnType, body);
}

// Parses the next statement by calling smtd on the first token else handle
// the declaration/assignment
Smt *ParseStatement(Parser *parser) {
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
	Token op = exp->node.binary.op;

	switch(op.type) {
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN:
			smt = newBinaryAssignmentSmt(left, op.type, right);
			break;
		case DEFINE:
			ASSERT(left->type == identExp, 
				"Expected left hand side of declare to be identifier");

			smt = newDeclareSmt(newVaribleDcl(left, NULL, right));
			
			// Added declaration to scope
			char *name = left->node.ident.name;
			Object *obj =(Object *)malloc(sizeof(Object));
			obj->name = name;
			obj->node = smt->node.declare;
			obj->type = varObj;
			obj->typeInfo = NULL;
			InsertScope(parser, name, obj);
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