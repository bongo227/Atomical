#include "includes/parser.h"

// new_parser creates a new parser
parser *new_parser(Token *tokens) {
	parser *p = (parser *)malloc(sizeof(parser));
	p->tokens = tokens;
	p->scope = parser_new_scope(NULL);
	p->ast = new_ast_unit();
	p->error_queue = new_queue(sizeof(parser_error));
	return p;
}

// parse_file creates an abstract sytax tree from the tokens in parser 
ast_unit *parse_file(parser *p) {
	Dcl **dcls = malloc(0);
	int dclCount = 0;
	while(p->tokens->type != END) {
		Dcl *d = parse_declaration(p);
		dcls = realloc(dcls, ++dclCount * sizeof(Dcl *));
		memcpy(dcls + dclCount - 1, &d, sizeof(Dcl *));
	}

	ast_unit *f = malloc(sizeof(ast_unit));
	f->dcls = dcls;
	f->dclCount = dclCount;

	return f;
}

// creates a new empty scope
scope *parser_new_scope(scope *outer) {
	scope *inner_scope = (scope *)malloc(sizeof(scope));
	inner_scope->outer = outer;
	scope_object *objects = NULL;
	inner_scope->objects = objects;

	return inner_scope;
}

// parser_enter_scope enters a new inner scope
void parser_enter_scope(parser *p) {
	p->scope = parser_new_scope(p->scope);
}

// parser_exit_scope exits the current scope
void parser_exit_scope(parser *p) {
	// clear hash table and free all scope objects
	scope_object *obj, *tmp;
	HASH_ITER(hh, p->scope->objects, obj, tmp) {
		HASH_DEL(p->scope->objects, obj);
		free(obj);
	}

	// Move to outer scipe
	scope *outer = p->scope->outer;
	free(p->scope);
	p->scope = outer;
}

// parser_insert_scope inserts an object into the current scope
bool parser_insert_scope(parser *p, char *name, Object *object) {
	// check if name is already in scope
	scope_object *obj;
	HASH_FIND_STR(p->scope->objects, name, obj);
	if (obj != NULL) return false;

	// add object to scope
	obj = (scope_object *)malloc(sizeof(scope_object));
	obj->name = name;
	obj->obj = object;
	HASH_ADD_KEYPTR(hh, p->scope->objects, obj->name, strlen(obj->name), obj);
	return true;
}

// parser_find_scope finds an object in scope
Object *parser_find_scope(parser *p, char *name) {
	scope_object *obj;
	for (scope *scope = p->scope; scope != NULL; scope = scope->outer) {
		HASH_FIND_STR(scope->objects, name, obj);
		if (obj != NULL) return obj->obj;
	}

	return NULL;
}

// parser_next moves the parser onto the next token
void parser_next(parser *p) {
	p->tokens++;
}

// parser_expect checks that the current token is of type type, if true parser advances, 
// else an error message is created.
Token *parser_expect(parser *p, TokenType type) {
	Token *token = p->tokens;
	if(token->type == type) {
		parser_next(p);
		return token;
	} else {
		new_error_token(p, type);
		return NULL;
	}
}

// parser_expect_semi expects a semicolon
void parser_expect_semi(parser *p) {
	if(p->tokens->type == SEMI || p->tokens->type == END) {
		parser_next(p);
	} else {
		new_error_token(p, SEMI);
	}
}

// new_error added a new error to the queue
parser_error *new_error(parser *p, parser_error_type type, int length) {
	parser_error *error = queue_push_back(p->error_queue);
	error->type = type;
	error->start = p->tokens;
	error->length = length;
	return error;
}

// new_error_token added a new token error to the queue
parser_error *new_error_token(parser *p, TokenType token_type) {
	parser_error *error = new_error(p, parser_error_expect_token, 1);
	error->expect_token.type = token_type;
	return error;
}

// parse_declaration parse a decleration node
Dcl *parse_declaration(parser *p) {
	switch(p->tokens->type) {
		case PROC:
			return parse_function_dcl(p);
		case VAR:
		case IDENT:
			return parse_variable_dcl(p);
		default: {
			// expected a top level declaration
			new_error(p, parser_error_expect_declaration, 1);
			return NULL;
		}
	}
}

Dcl *parse_declaration_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_declaration(p);
}

void parser_skip_next_block(parser *p) {
		// Move to start of block
		while(p->tokens->type != LBRACE) p->tokens++;
		
		// Skip over block (and all sub blocks)
		int depth = 0;
		do {
			if(p->tokens->type == LBRACE) depth++;
			else if(p->tokens->type == RBRACE) depth--;
			p->tokens++;
		} while(depth > 0);

		if(p->tokens->type == SEMI) p->tokens++;
}

void parser_skip_to_semi(parser *p) {
	// Move past first semi
	while(p->tokens->type != SEMI && p->tokens->type != END) p->tokens++;
	if(p->tokens->type == SEMI) p->tokens++;
}

// parse_function_dcl parses a function decleration
Dcl *parse_function_dcl(parser *p) {
	// Parse proc
	Token *proc = parser_expect(p, PROC);
	if (proc == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// Parse function name
	Token *ident = parser_expect(p, IDENT);
	if (ident == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}
	char *name = ident->value; // function name
	
	// Parse argument seperator
	parser_expect(p, DOUBLE_COLON);
	// missing double colon is not fatel so countinue

	// Parse arguments
	Dcl *args = (Dcl *)malloc(0);
	int argCount = 0;
	while(p->tokens->type != ARROW && p->tokens->type != LBRACE) {
		if (argCount > 0) parser_expect(p, COMMA);
		// missing comma not fatel

		args = realloc(args, sizeof(Dcl) * ++argCount);

		// Construct argument
		Exp *type = parse_type(p); // arg type
		if (type == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}

		// arg name
		Token *name_token = parser_expect(p, IDENT); 
		if (name_token == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}
		char *name = name_token->value; 

		// add argument to list
		Dcl *arg = new_argument_dcl(p->ast, type, name);
		void *dest = memcpy(args + argCount - 1, arg, sizeof(Dcl));
	}
	
	Token *arrow = parser_expect(p, ARROW);
	if (arrow == NULL) {
		// arrow fatel since we dont know the return type
		parser_skip_next_block(p);
		return NULL;
	}

	Exp *return_type = parse_type(p);
	if (return_type == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// insert arguments into scope
	for (int i = 0; i < argCount; i++) {
		// insert into scope
		Object *obj = (Object *)malloc(sizeof(Object));
		obj->name = args[i].argument.name;
		obj->node = args + i;
		obj->type = argObj;
		parser_insert_scope(p, obj->name, obj);
	}

	// insert function into scope
	Dcl* function = new_function_dcl(p->ast, name, args, argCount, return_type, NULL);
	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = function;
	obj->type = funcObj;
	parser_insert_scope(p, name, obj);
	
	// parse body
	Smt *body = parse_block_smt(p);
	function->function.body = body;

	if(p->tokens->type == SEMI) p->tokens++;

	return function;
}

Dcl *parse_variable_dcl(parser *p) {
	char *name;
	Exp *type = NULL;
	Exp *value;

	if(p->tokens->type == VAR) {
		p->tokens++;
		
		// Type
		type = parse_type(p);
		if (type == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}

		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;

		// Assign
		parser_expect(p, ASSIGN);
		// non fatel

		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	} else {
		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;
		
		// Define
		parser_expect(p, DEFINE);
		// non fatel
		
		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	}

	Dcl *dcl = new_varible_dcl(p->ast, name, type, value);

	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = dcl;
	obj->type = varObj;
	parser_insert_scope(p, name, obj);

	return dcl;
}

// Parses the next statement by calling smtd on the first token else handle
// the declaration/assignment
Smt *parse_statement(parser *p) {
	Token *t = p->tokens;
	Smt *smt = smtd(p, t);
	if (smt != NULL) {
		return smt;
	}

	// Statement is an assignment/declaration, so treat it like an expression
	// and transform it.
	Exp *exp = parse_expression(p, 0);
	if(exp == NULL) {
		queue_pop_back(p->error_queue); // remove expression error
		new_error(p, parser_error_expect_statement, 1);
	}

	// Expected assigment/declation statement
	assert(exp->type == binaryExp); 
	
	Exp *left = exp->binary.left;
	Exp *right = exp->binary.right;
	Token op = exp->binary.op;

	switch(op.type) {
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN:
			smt = new_binary_assignment_smt(p->ast, left, op.type, right);
			break;
		case DEFINE:
			assert(left->type == identExp);

			char *name = left->ident.name;
			smt = new_declare_smt(p->ast, new_varible_dcl(p->ast, name, NULL, right));
			
			// Added declaration to scope
			Object *obj =(Object *)malloc(sizeof(Object));
			obj->name = name;
			obj->node = smt->declare;
			obj->type = varObj;
			parser_insert_scope(p, name, obj);
			break;
		default:
			// Expected an assignment operator
			assert(false);
	}

	// If statment is null, the next tokens dont start a valid statement
	assert(smt != NULL);

	// Release the converted expression back into the pool
	pool_release(p->ast->exp_pool, exp);
	
	return smt;
}

Smt *parse_statement_from_string(char *src) {
	parser *p = new_parser(Lex(src));
    return parse_statement(p);
}

Smt *parse_block_smt(parser *p) {
	parser_expect(p, LBRACE);
	parser_enter_scope(p);

	// build list of statements
	int smtCount = 0;
	Smt *smts = (Smt *)malloc(sizeof(Smt) * 1024);
	Smt *smtsPrt = smts;
	while(p->tokens->type != RBRACE) {
		smtCount++;
		memcpy(smtsPrt, parse_statement(p), sizeof(Smt));
		if(p->tokens->type != RBRACE) parser_expect_semi(p);
		smtsPrt++;
	}
	smts = realloc(smts, sizeof(Smt) * smtCount);

	parser_expect(p, RBRACE);
	parser_exit_scope(p);

	Smt *s = new_block_smt(p->ast, smts, smtCount);
	return s;
}

// smtd parser the current token in the context of the start of a statement
Smt *smtd(parser *p, Token *token) {
	switch(token->type) {
		// return statement
		case RETURN: {
			p->tokens++;
			Smt *s = new_ret_smt(p->ast, parse_expression(p, 0));
			return s; 
		}
		// block statement
		case LBRACE:
			return parse_block_smt(p);
		
		// if statement
		case IF: {
			p->tokens++;
			
			Exp *cond = parse_expression(p, 0);
			Smt *block = parse_statement(p);
			assert(block->type == blockSmt);
			Smt *elses = NULL;

			// Check for elseif/else
			if (p->tokens->type == ELSE) {
				p->tokens++;
				if (p->tokens->type == IF) {
					// else if, so recursivly parse else chain
					elses = parse_statement(p);
				} else {
					// final else statment only has a body
					elses = new_if_smt(p->ast, NULL, parse_statement(p), NULL);
				}
			}

			return new_if_smt(p->ast, cond, block, elses);
		}
		// for loop
		case FOR: {
			p->tokens++;

			// parse index
			Dcl *index = parse_declaration(p);
			assert(index->type == varibleDcl);
			parser_expect_semi(p);

			// parse condition
			Exp *cond = parse_expression(p, 0);
			parser_expect_semi(p);

			// parse increment
			Smt *inc = parse_statement(p);
			
			// parse body
			Smt *body = parse_statement(p);
			assert(body->type == blockSmt);

			return new_for_smt(p->ast, index, cond, inc, body);
		}
		// varible declaration
		case VAR: {
			return new_declare_smt(p->ast, parse_variable_dcl(p));
		}
		// increment expression
		case IDENT: {
			Exp *ident = parse_ident_exp(p);

			Token one_token = {INT, 0, 0, "1"};
			Exp *one_literal = new_literal_exp(p->ast, one_token);

			switch(p->tokens->type) {
				case INC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, ADD_ASSIGN, one_literal);
				case DEC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, SUB_ASSIGN, one_literal);
				default:
					// expression is assigment or declaration so let caller handle it
					p->tokens--; // go back to ident
					return NULL;
			}
		}
		default:
			// Expected a statement
			assert(false);
	}
	return NULL;
}

// Parses the next expression by binding tokens until the left binding power is 
// <= right binding power (rbp)
Exp *parse_expression(parser *p, int rbp) {
	Exp *left;
	Token *t = p->tokens;
	parser_next(p);
	left = nud(p, t);
	if(left == NULL) return NULL;
	while (rbp < get_binding_power(p->tokens->type)) {
		t = p->tokens;
		parser_next(p);
		left = led(p, t, left);
		if(left == NULL) return NULL;
	}
	return left;
}

Exp *parse_expression_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_expression(p, 0); 
}

// nud parses the current token in a prefix context (at the start of an (sub)expression)
Exp *nud(parser *p, Token *token) {
	switch (token->type) {
		case IDENT:
			return parse_ident_exp_from_token(p, token);
		
		case INT:
		case FLOAT:
		case HEX:
		case OCTAL:
		case STRING:
			return new_literal_exp(p->ast, *token);

		case NOT:
		case SUB:
			return new_unary_exp(p->ast, *token, parse_expression(p, 60));

		case LBRACE:
			return parse_key_value_list_exp(p);

		case LBRACK:
			return parse_array_exp(p);

		default: {
			// Expected a prefix token
			new_error(p, parser_error_expect_prefix, 1);
			return NULL;
		}
	}

	return NULL;
}

// led parses the current token in a infix contect (between two nodes)
Exp *led(parser *p, Token *token, Exp *exp) {
	int bp = get_binding_power(token->type);
	
	switch (token->type) {
		// binary expression
		case ADD:
		case SUB:
		case MUL:
		case QUO:
		case REM:
		case EQL:
		case NEQ:
		case GTR:
		case LSS:
		case GEQ:
		case LEQ: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp));
		}

		// selector expression
		case PERIOD: {
			return new_selector_exp(p->ast, exp, parse_expression(p, bp));
		}

		// index expression
		case LBRACK: {
			Exp *index = parse_expression(p, 0);
			parser_expect(p, RBRACK);
			
			return new_index_exp(p->ast, exp, index);
		}

		// array/struct expression
		case LBRACE: {
			printf("LBrace!");
		}

		// call expression
		case LPAREN: {
			int argCount = 0;
			Exp *args = (Exp *)malloc(0);
			if(p->tokens->type != RPAREN) {
				// arguments are not empty so parse arguments
				while(true) {
					argCount++;
					
					args = realloc(args, argCount * sizeof(Exp));
					Exp *arg = parse_expression(p, 0);
					memcpy(args + argCount - 1, arg, sizeof(Exp));
					
					if(p->tokens->type == RPAREN) break;
					parser_expect(p, COMMA);
				}
			}
			parser_expect(p, RPAREN);

			return new_call_exp(p->ast, exp, args, argCount);
		}

		// right associative binary expression or assignments
		// if the expression is an assigment, return a binary statement and let parse_statement 
		// transform it into a statement
		case LAND:
		case LOR:
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN: 
		case DEFINE: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp - 1));	
		}
		default: {
			// expected an infix expression
			new_error(p, parser_error_expect_infix, 1);
			return NULL;
		}
	}

	return NULL;
}

// Parses key value expressions in the form "expression:expression" or "expression"
Exp *parse_key_value_exp(parser *p) {
	Exp *keyOrVal = parse_expression(p, 0);
	Exp *key = NULL;
	Exp *value = NULL;
	
	if (p->tokens->type == COLON) {
		// Key/value belongs to structure expression
		p->tokens++;
		key = keyOrVal;
		value = parse_expression(p, 0);
	} else {
		// Key/value belongs to array expression
		value = keyOrVal;
	}

	return new_key_value_exp(p->ast, key, value);
}

Exp *parse_key_value_list_exp(parser *p) {
	int keyCount = 0;
	Exp *values = malloc(0);

	while(p->tokens->type != RBRACE) {
		keyCount++;
		values = realloc(values, keyCount * sizeof(Exp));
		Exp *keyValue = parse_key_value_exp(p);
		memcpy(values + keyCount - 1, keyValue, sizeof(Exp));
		
		if(p->tokens->type != RBRACE) parser_expect(p, COMMA);
	}

	return new_key_value_list_exp(p->ast, values, keyCount);
}

Exp *parse_array_exp(parser *p) {
	int valueCount = 0;
	Exp *values = malloc(0);
	while(p->tokens->type != RBRACK) {
		values = realloc(values, (++valueCount) * sizeof(Exp));
		Exp *value = parse_expression(p, 0);
		memcpy(values + valueCount - 1, value, sizeof(Exp));
		if (p->tokens->type != RBRACK) parser_expect(p, COMMA);
	}

	parser_expect(p, RBRACK);

	return new_array_exp(p->ast, values, valueCount);
}

// parse_type parses a type, adds an error if no type was found
Exp *parse_type(parser *p) {
	Exp *ident = parse_ident_exp(p);
	if (ident == NULL) {
		queue_pop_back(p->error_queue); // discard ident error
		new_error(p, parser_error_expect_type, 1);
		return NULL;
	}

	if(p->tokens->type == LBRACK) {
		// Type is an array type
		p->tokens++;
		Exp *length = parse_expression(p, 0);
		if(length == NULL) {
			new_error(p, parser_error_expect_array_length, 1);
			return NULL;
		}

		parser_expect(p, RBRACK);
		return new_array_type_exp(p->ast, ident, length);
	}

	return ident;
}

Exp *parse_ident_exp_from_token(parser *p, Token *token) {
	if(token->type != IDENT){
		// Add error to queue
		new_error_token(p, IDENT);
		return NULL;
	}

	char *name = token->value;
	Exp *ident = new_ident_exp(p->ast, name);
	Object *obj = parser_find_scope(p, name);
	ident->ident.obj = obj;
	
	return ident;
}

Exp *parse_ident_exp(parser *p) {
	Exp *ident = parse_ident_exp_from_token(p, p->tokens);
	parser_next(p);
	return ident;
}
