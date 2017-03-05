#include "ast.h"

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