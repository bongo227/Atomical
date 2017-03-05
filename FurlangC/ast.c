#include "ast.h"

Exp *makeIdentExp(char *ident) {
    Exp *e = malloc(sizeof(Exp));
    e->type = identExp;
    e->node.ident.name = ident;
    e->node.ident.obj = NULL;

    return e;
}

Exp *literalExp(Token literal) {
    Exp *e = malloc(sizeof(Exp));
    e->type = literalExp;
    e->node.literal = literal;

    return e;
}

Exp *parenExp(Exp *exp) {
    Exp *e = malloc(sizeof(Exp));
    e->type = parenExp;
    e->node.paren = exp;

    return e;
}

Exp *selectorExp(Exp *exp, Exp *selector) {
    Exp *e = malloc(sizeof(Exp));
    e->type = selectorExp;
    e->node.selector.exp = exp;
    e->node.selector.selector = selector;

    return e;
}

Exp *starExp(Exp *exp) {
    Exp *e = malloc(sizeof(Exp));
    e->type = starExp;
    e->node.star = exp;

    return e;
}

Exp *unaryExp(Token op, Exp *exp) {
    Exp *e = malloc(sizeof(Exp));
    e->type = unaryExp;
    e->node.unary.op = op;
    e->node.unary.right = exp;

    return e;
}

Exp *braceLiteralExp(Token lBrace, Exp *exp, int expCount, Token rBrace) {
    Exp *e = malloc(sizeof(Exp));
    e->type = braceLiteralExp;
    e->node.braceLiteral.lBrace = lBrace;
	e->node.braceLiteral.exp = exp;
	e->node.braceLiteral.expCount = expCount;
	e->node.braceLiteral.rBrace = rBrace;

    return e;
}