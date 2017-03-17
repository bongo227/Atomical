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
}

void vlog(const char *format, va_list argp) {
    printf("\e[32m[          ]\e[0m   \e[2m");
    vprintf(format, argp);
    printf("\e[0m\n");
}

void log(const char *format, ...) {
    va_list argp;
    va_start(argp, format);
    vlog(format, argp);
    va_end(argp);
}

// test files
#include "lexer_test.cpp"
#include "parser_test.cpp"
#include "irgen_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
