#include "error.c"
#include "lexer.c"
#include "pool.c"
#include "queue.c"
#include "ast.c"
#include "parser.c"
#include "irgen.c"

// TODO: link this against furlib

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