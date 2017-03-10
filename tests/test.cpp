#include <gtest/gtest.h>

// src project
#include "../src/lexer.c"
#include "../src/ast.c"
#include "../src/parser.c"
#include "../src/irgen.c"

// test files
#include "lexer_test.cpp"
#include "parser_test.cpp"
#include "irgen_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
