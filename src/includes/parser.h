#pragma once

#include "uthash.h"
#include "all.h"
#include "queue.h"

#define ERROR_QUEUE_SIZE 10
#define MAX_ERRORS 10

typedef struct {
	char *name;
	Object *obj;
	UT_hash_handle hh;
} scope_object;

struct _scope {
	struct _scope *outer;
	scope_object *objects;
};
typedef struct _scope scope;

typedef struct {
	scope *scope;
    Token *tokens;
	ast_unit *ast;
	queue *error_queue; // this should be a list so we can remove errors from the end to add more context
} parser;

typedef enum {
	parser_error_expect_token,
	parser_error_expect_declaration,
	parser_error_expect_statement,
	parser_error_expect_expression,
	parser_error_expect_type,
	parser_error_expect_array_length,
	parser_error_expect_block,
	parser_error_expect_prefix,
	parser_error_expect_infix,
} parser_error_type;

typedef struct {
	parser_error_type type;
	Token *start;
	int length;

	union {
		struct {
			TokenType type;
		} expect_token;
	};
} parser_error;

// Parser interface
parser *new_parser(Token *tokens);
ast_unit *parse_file(parser *parser);

// Scope
scope *parser_new_scope(scope *outer);
void parser_enter_scope(parser *parser);
void parser_exit_scope(parser *parser);
bool parser_insert_scope(parser *parser, char *name, Object *object);
Object *parser_find_scope(parser *parser, char *name);

// Helpers
void parser_next(parser *parser);
Token *parser_expect(parser *parser, TokenType type);
void parser_expect_semi(parser *parser);
parser_error *new_error(parser *p, parser_error_type type, int length);
parser_error *new_error_token(parser *p, TokenType token_type);

// Declarations
Dcl *parse_declaration(parser *parser);
Dcl *parse_function_dcl(parser *parser);
Dcl *parse_variable_dcl(parser *parser);

// Statements
Smt *parse_statement(parser *parser);
Smt *parse_statement_from_string(char *src);
Smt *parse_block_smt(parser *p);
Smt *smtd(parser *p, Token *token);

// Expressions
Exp *parse_expression(parser *parser, int rbp);
Exp *parse_expression_from_string(char *src);
Exp *nud(parser *parser, Token *token);
Exp *led(parser *parser, Token *token, Exp *exp);
Exp *parse_key_value_exp(parser *parser);
Exp *parse_key_value_list_exp(parser *parser);
Exp *parse_array_exp(parser *parser);
Exp *parse_type(parser *parser);
Exp *parse_ident_exp_from_token(parser *parser, Token *token);
Exp *parse_ident_exp(parser *parser);