#include "includes/error.h"

int main() {
	char *src = "proc add :: int a, int b -> int {\n\treturn a + c\n}";

	error(
		src,
		1,
		6,
		8,
		"Function with name \"%s\" already defined",
		"add");
	
	error(
		src,
		2,
		12,
		12,
		"Varible \"%s\" is not defined",
		"a");

	return 0;
}