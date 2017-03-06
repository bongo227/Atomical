#include "fur.h"

Exp *newIdentExp(char *ident) {
    Exp *e = (Exp *)malloc(sizeof(Exp));
    e->type = identExp;
    e->node.ident.name = ident;
    e->node.ident.obj = NULL;

    return e;
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
	Token t = {op, 0, 0, ""};
	e->node.assignment.right = newBinaryExp(left, t, right);

	return e; 
}

Smt *newAddAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, ADD, right);
}

Smt *newSubAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, SUB, right);
}

Smt *newMulAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, MUL, right);
}

Smt *newRemAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, REM, right);
}

Smt *newOrAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, OR, right);
}

Smt *newShlAssignmentSmt(Exp *left, Exp *right) {
	return newBinaryAssignmentSmt(left, SHL, right);
}

Smt *newReturnSmt(Exp *result) {
	Smt *s = (Smt *)malloc(sizeof(Smt));
	s->type = returnSmt;
	s->node.ret.result = result;
	
	return s;
}