#pragma once

#include "ast.h"
#include "parser.h"

typedef struct {
    Token *tokens;
    int expLevel; // >= 0 when inside expression
    bool rhs; // true if parser is parsing right hand side
} Parser;