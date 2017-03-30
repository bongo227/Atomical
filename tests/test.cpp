#include <gtest/gtest.h>

#include <stdio.h>
#include <stdarg.h>

// src project
extern "C" {
    #include "../src/includes/error.h"
    #include "../src/includes/lexer.h"
    #include "../src/includes/ast.h"
    #include "../src/includes/parser.h"
    #include "../src/includes/irgen.h"
    #include "../src/includes/pool.h"
}

// test files
#include "lexer_test.cpp"
#include "parser_test.cpp"
#include "irgen_test.cpp"
#include "pool_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    // ::testing::GTEST_FLAG(filter) = "*Pool";
    return RUN_ALL_TESTS();
}
