#include "unity.h"
#include "lex.h"

void test_lex_ident() {
	Token *tokens = Lex("test");

	TEST_ASSERT_EQUAL_INT(IDENT, tokens[0].type);
	TEST_ASSERT_EQUAL_STRING("test", tokens[0].value);

	TEST_ASSERT_EQUAL_INT(END, tokens[1].type);
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_lex_ident);
	return UNITY_END();
}