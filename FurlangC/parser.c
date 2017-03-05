#include "parser.h"

void next(Parser *parser) {
    parser->tokens++;
}

void expect(Parser *parser, TokenType type) {
    if (parser->tokens->type != type) {
        printf("Expected: %d, got: %d", type, parser->tokens->type);
    }
    next(parser);
}

// <ident> ::= <ident>
Exp *parseIdent(Parser *parser) {
    char *name = "_";
    if (parser->tokens->type == IDENT) {
        name = parser->tokens->value;
        next(parser);
    } else {
        expect(parser, IDENT);
    }

    return makeIdentExp(name);
}

// expects the expression to be a non-type expression
Exp *expectExp(Exp *exp) {
    switch(exp->type) {
        case identExp:
        case literalExp:
        case parenLiteral:
        case indexExp:
        case sliceExp:
        case callExp:
        case unaryExp:
        case binaryExp:
            break;
        default:
            // all other nodes are type-expressions
            printf("Expected expression, got type");
    }

    return exp;
}

Exp *expectExpOrType(Exp *exp) {
    // TODO: finish this
    return exp;
}

// tryResolve looks up the object an identifier represents, if no object is found and mark is
// is true the identifier is added to a list of unresolved identifiers.
void tryResolve(Exp *exp, mark bool) {
    if (exp->type != IDENT) return; // only identifers are resolved
    assert(exp->node.ident.obj == NULL);
    if (*exp->node.ident.name == '_') return; // dont resolve black identifiers

    // TODO: finish this
    // look through scoped_ptr
    // add to list
}

void resolve(Exp *exp) {
    tryResolve(exp, true);
}

// <typeName> ::= <ident> || <selectorExpression>
Exp *parseTypeName(Parser *parser) {
    printf("TypeName");

    Exp *ident = parseIdent(parser)

    if (parser->tokens->type == PERIOD) {
        // type name is in package
        next(parser);
        resolve(parser, ident);
        Exp *selector = parseIdent(parser);
        return selectorExp(ident, selector);
    }

    return ident;
}

// trys to parse tokens as type
Exp *tryType(Parser *parser) {
    switch (parser->tokens->type) {
        case IDENT:
            return parseTypeName() 
        // TODO: add other types
    }

    return NULL;
}

Exp *parseOperand(Parser *parser, bool lhs) {
    printf("Operand");

    switch(parser->tokens->type) {
        case IDENT:
            Exp *ident = parseIdent()
            if (!lhs) {
                resolve(ident);
            }
            return ident;

        case INT:
        case FLOAT:
        case HEX:
        case OCTAL:
        case STRING:
            return literalExp(parser->tokens);

        case LPAREN:
            next(parser);
            parser->expLevel++;
            Exp *exp = parseRhsOrType();
            parser->expLevel--;
            expect(RPAREN);
            return parenExp(exp);
    }

    Exp *type = tryType(parser);
    if (type != NULL) {
        if (type->type == identExp) printf("type cannot be identifier");
        return type;
    }

    printf("Expected operand");
}

Exp *parsePrimaryExp(Parser *parser, bool lhs) {
    printf("Primary Expression");

    Exp *exp = parseOperand(parser, lhs)
loop:
    while(true) {
        switch (parser->tokens->type) {
            case PERIOD:
                break loop;
        }
    }
}

Exp *parseUnaryExp(Parser *parser, bool lhs) {
    switch (parser->tokens->type) {
        case ADD:
        case SUB:
        case NOT:
        case XOR:
        case AND:
            next(parser);
            Exp exp = parseUnaryExp(false)
            return unaryExp(*parser->tokens, expectExp(exp));
        case MUL:
            // pointer or unary *
            next(parser);
            Exp exp = parserUnaryExp(false);
            return starExp(exp);
    }

    return parsePrimaryExp(parser, lhs)
}

Exp *parseBinaryExp(Parser *parser, bool lhs, int prec) {
    printf("Binary Expression");
    
}

Exp *parseExp(Parser *parser, bool lhs) {
    printf("Expression");
    return parseBinaryExp(parser, lhs, LowPrec+1);
}

Exp *parseRhs(Parser *parser) {
    // save previous rhs state
    bool rhs = parser->rhs;
    
    parser->rhs = true;
    Exp *exp = expectExp(parseExp(false));

    // restore previous rhs state
    parser->rhs = rhs;
    
    return exp;
}

Exp *parseRhsOrType() {
    // save previous rhs state
    bool rhs = parser->rhs;
    
    parser->rhs = true;
    Exp *exp = expectExpOrType(parseExp(false));

    // restore previous rhs state
    parser->rhs = rhs;
    
    return exp;
}

Exp *parseIndexOrSlice(Parser *parser) {
    expect(parser, LBRACK);
    parser->expLevel++;

    Exp index[3] = {};
    index[0] = parseRhs();
}