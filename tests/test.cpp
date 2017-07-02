#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "../src/lib.cpp"
#include "lexer_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}