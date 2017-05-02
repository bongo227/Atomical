#include "includes/error.h"
#include "includes/lexer.h"
#include "includes/ast.h"
#include "includes/parser.h"
#include "includes/irgen.h"
#include "includes/parser.h"
#include "includes/string.h"


#include <llvm-c/BitWriter.h>
#include <stdlib.h>

void print_usage() {
	printf("usage: fur filename <flags>\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	string in_file = string_new(argv[1]);
	string out_file = string_new("");
	bool emit_tokens = false;
	bool emit_ircode = false;

	// Check for no/incorrect input file
	if (argc < 2 || argv[0][0] == '-') print_usage();
	
	// Check input file has .fur extension
	int in_len = string_length(in_file);
	if(in_len < 4) print_usage();
	if(strcmp(in_file + in_len - 4, ".fur") != 0) print_usage();
	
	// Create default out name
	out_file = string_append_length(out_file, in_file, in_len - 4);
	out_file = string_append_cstring(out_file, ".ll");
	
	// Parse tokens
	for (int i = 2; i < argc; i++) {
		if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0) {
			if(i == argc - 1) print_usage();
			i++;
			if(argv[i][0] == '-') print_usage();
			out_file = argv[i];
			i++;
		} else {
			emit_tokens = emit_tokens || strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tokens") == 0;
			emit_ircode = emit_ircode || strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ircode") == 0;
		}
	}

	// Open the in and out files
	FILE *in_file_hdl = fopen(in_file, "r");
	FILE *out_file_hdl = fopen(out_file, "w+");

	// Copy file content to string buffer
	string buffer = string_new_file(in_file_hdl);
	printf("Done\n");

	// Compile the file
	Token *tokens = Lex(buffer);
	printf("Lexer done\n");
	parser *p = new_parser(tokens);
	ast_unit *ast = parse_file(p);
	printf("Parser done\n");
	Irgen *irgen = NewIrgen();
	printf("Irgen done\n");
	for (int i = 0; i < ast->dclCount; i++) {
        CompileFunction(irgen, ast->dcls[i]);
    }
	printf("Compiled to LLVM\n");

	// Write the file to llvm bitcode
	int rc = LLVMWriteBitcodeToFD(irgen->module, fileno(out_file_hdl), true, true); // out_file_hdl closed here
	if (rc > 0) {
		printf("LLVM to bitcode error\n");
		exit(1);
	}
	printf("Compile to bitcode\n");
	
	// Compile to assembly
	string llc_command = string_new("llc-3.9 ");
	llc_command = string_append(llc_command, out_file);
	system(llc_command);
	printf("Compiled to assembly\n");
	string llc_file = string_copy(out_file);
	llc_file = string_slice(llc_file, 0, string_length(llc_file) - 3); // remove .ll
	llc_file = string_append_cstring(llc_file, ".s"); // add .s

	// Create executable
	string clang_command = string_new("clang-3.9 ");
	clang_command = string_append(clang_command, llc_file);
	system(clang_command);
	printf("Executable created\n");

	// Remove temporary files
	string rm_command = string_new("rm ");
	rm_command = string_append(rm_command, out_file);
	rm_command = string_append_cstring(rm_command, " ");
	rm_command = string_append(rm_command, llc_file);
	system(rm_command);
	printf("Removed temporary files\n");

	fclose(in_file_hdl);

	return 0;
}