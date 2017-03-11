#include "error.c"
#include "lexer.c"
#include "ast.c"
#include "parser.c"
#include "irgen.c"

int main() {
	Error(
		"proc add :: int a, int b -> int { return a + b }",
		23,
		6,
		8,
		"Function with name \"%s\" already defined",
		"add");
	return 0;
}