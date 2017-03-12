#include "includes/ast.h"

Exp *newIdentExp(char *ident) {
    Exp *e = (Exp *)malloc(sizeof(Exp));
    e->type = identExp;
    e->node.ident.name = ident;
    e->node.ident.obj = NULL;

    return e;
}

Exp *newLiteralExp(Token lit) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = literalExp;
	e->node.literal = lit;

	return e;
}

Exp *newIntLiteral(char *n) {
	Token t = {INT, 0, 0, n};
	return newLiteralExp(t);
}

Exp *newBinaryExp(Exp *left, Token op, Exp *right) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = binaryExp;
	e->node.binary.left = left;
	e->node.binary.op = op;
	e->node.binary.right = right;

	return e;
}

Exp *newSelectorExp(Exp *exp, Exp* selector) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = selectorExp;
	e->node.selector.exp = exp;
	e->node.selector.selector = selector;

	return e;
}

Exp *newIndexExp(Exp *exp, Exp *index) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = indexExp;
	e->node.index.exp = exp;
	e->node.index.index = index;

	return e;
}

Exp *newUnaryExp(Token op, Exp *right) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = unaryExp;
	e->node.unary.op = op;
	e->node.unary.right = right;

	return e;
}

Smt *newAssignmentSmt(Exp *left, Exp *right) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = assignmentSmt;
	s->node.assignment.left = left;
	s->node.assignment.right = right;

	return s;
}

Smt *newBinaryAssignmentSmt(Exp *left, TokenType op, Exp *right) {
	Smt *e = (Smt *)malloc(sizeof(Smt));
	e->type = assignmentSmt;
	e->node.assignment.left = left;
	
	Token t = {ADD, 0, 0, ""};
	
	switch(op) {
		case ASSIGN:
			break;
		case ADD_ASSIGN:
			t.type = ADD;
			right = newBinaryExp(left, t, right);
			break;
		case SUB_ASSIGN:
			t.type = SUB; 
			right = newBinaryExp(left, t, right);
			break;
		case MUL_ASSIGN:
			t.type = MUL;
			right = newBinaryExp(left, t, right);
			break;
		case REM_ASSIGN:
			t.type = REM;
			right = newBinaryExp(left, t, right);
			break;
		case OR_ASSIGN:
			t.type = OR; 
			right = newBinaryExp(left, t, right);
			break;
		case SHL_ASSIGN:
			t.type = SHL;
			right = newBinaryExp(left, t, right);
			break;
	}

	e->node.assignment.right = right;
	return e; 
}

Smt *newReturnSmt(Exp *result) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = returnSmt;
	s->node.ret.result = result;
	
	return s;
}

Smt *newBlockSmt(Smt *smts, int smtCount) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = blockSmt;
	s->node.block.smts = smts;
	s->node.block.count = smtCount;

	return s; 
}

Smt *newIfSmt(Exp *cond, Smt *body, Smt *elses) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = ifSmt;
	s->node.ifs.cond = cond;
	s->node.ifs.body = body;
	s->node.ifs.elses = elses;

	return s;
}

Dcl *newVaribleDcl(Exp *name, Exp *type, Exp *value) {
	Dcl *d = (Dcl *)malloc(sizeof(Dcl));
	d->type = varibleDcl;
	d->node.varible.name = name;
	d->node.varible.type = type;
	d->node.varible.value = value;

	return d;
}

Smt *newDeclareSmt(Dcl *dcl) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = declareSmt;
	s->node.declare = dcl;

	return s; 
}

Exp *newArrayTypeExp(Exp *type, Exp *length) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = arrayTypeExp;
	e->node.arrayType.type = type;
	e->node.arrayType.length = length;

	return e;
}

Exp *newFieldTypeExp(Exp *type, Exp *name) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = fieldTypeExp;
	e->node.fieldType.type = type;
	e->node.fieldType.name = name;

	return e;
}

Exp *newStructTypeExp(Exp *fields, int count) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = structTypeExp;
	e->node.structType.fields = fields;
	e->node.structType.feildCount = count;

	return e;
}

Dcl *newArgumentDcl(Exp *type, Exp *name) {
	Dcl *d = (Dcl *)malloc(sizeof(Dcl));
	d->type = argumentDcl;
	d->node.argument.type = type;
	d->node.argument.name = name;

	return d;
}

Dcl *newFunctionDcl(Exp *name, Dcl *args, int argCount, Exp *returnType, Smt *body) {
	Dcl *d = (Dcl *)malloc(sizeof(Dcl));
	d->type = functionDcl;
	d->node.function.name = name;
	d->node.function.args = args;
	d->node.function.argCount = argCount;
	d->node.function.returnType = returnType;
	d->node.function.body = body;

	return d;
}

Exp *newCallExp(Exp *function, Exp *args, int argCount) {
	Exp *e = (Exp *)malloc(sizeof(Exp));
	e->type = callExp;
	e->node.call.function = function;
	e->node.call.args = args;
	e->node.call.argCount = argCount;

	return e;
} 

Smt *newForSmt(Dcl *index, Exp *cond, Smt *inc, Smt *body) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = forSmt;
	s->node.fors.index = index;
	s->node.fors.cond = cond;
	s->node.fors.inc = inc;
	s->node.fors.body = body;

	return s;
}