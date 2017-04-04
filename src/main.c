#include "includes/error.h"

void print_usage() {
	printf("usage: fur filename <flags>\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	char *in_file = argv[1];
	char *out_file = "";
	bool tokens = false;
	bool ircode = false;

	// Check for no/incorrect input file
	if (argc < 2 || argv[0][0] == '-') print_usage();
	
	// Check input file has .fur extension
	int in_len = strlen(in_file);
	if(in_len < 4) print_usage();
	if(strcmp(in_file + in_len - 4, ".fur") != 0) print_usage();
	
	// Create default out name
	out_file = malloc(in_len - 3 * sizeof(char));
	memcpy(out_file, in_file, (in_len - 4) * sizeof(char));
	out_file[in_len - 3] = '\0';
	
	// Parse tokens
	for (int i = 2; i < argc; i++) {
		if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
			if(i == argc - 1) print_usage();
			i++;
			if(argv[i][0] == '-') print_usage();
			out_file = argv[i];
			i++;
		} else {
			tokens = tokens || strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "-tokens") == 0;
			ircode = ircode || strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-ircode") == 0;
		}
	}

	printf("In: %s\nOut: %s\nTokens: %d\nIrcode: %d\n", in_file, out_file, tokens, ircode);
	
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