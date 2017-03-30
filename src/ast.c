#include "includes/ast.h"

// Exp *new_int_literal_exp(char *n) {
// 	Token t = {INT, 0, 0, n};
// 	return newLiteralExp(t);
// }

ast_unit *new_ast_unit() {
	ast_unit *ast = malloc(sizeof(ast_unit));
	ast->dcl_pool = new_pool(sizeof(Dcl), 128);
	ast->smt_pool = new_pool(sizeof(Smt), 128);
	ast->exp_pool = new_pool(sizeof(Exp), 128);
	ast->dcls = malloc(0);
	ast->dclCount = 0;

	return ast;
}

Exp *new_ident_exp(ast_unit *ast, char *ident) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = identExp;
	e->ident.name = ident;
	e->ident.obj = NULL;

	return e;
}

Exp *new_literal_exp(ast_unit *ast, Token lit) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = literalExp;
	e->literal = lit;

	return e;
}

Exp *new_unary_exp(ast_unit *ast, Token op, Exp *right) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = unaryExp;
	e->unary.op = op;
	e->unary.right = right;

	return e;
}

Exp *new_binary_exp(ast_unit *ast, Exp *left, Token op, Exp *right) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = binaryExp;
	e->binary.left = left;
	e->binary.op = op;
	e->binary.right = right;

	return e;
}

Exp *new_selector_exp(ast_unit *ast, Exp *exp, Exp* selector) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = selectorExp;
	e->selector.exp = exp;
	e->selector.selector = selector;

	return e;
}

Exp *new_index_exp(ast_unit *ast, Exp *exp, Exp *index) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = indexExp;
	e->index.exp = exp;
	e->index.index = index;

	return e;
}

Exp *new_slice_exp(ast_unit *ast, Exp *exp, Exp *low, Exp *high) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = sliceExp;
	e->slice.exp = exp;
	e->slice.low = low;
	e->slice.high = high;

	return e;
}

Exp *new_star_exp(ast_unit *ast, Exp *exp) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = starExp;
	e->star = exp;

	return e;
}

Exp *new_call_exp(ast_unit *ast, Exp *function, Exp *args, int argCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = callExp;
	e->call.function = function;
	e->call.args = args;
	e->call.argCount = argCount;

	return e;
}

Exp *new_key_value_exp(ast_unit *ast, Exp *key, Exp *value) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = keyValueExp;
	e->keyValue.key = key;
	e->keyValue.value = value;

	return e;
}

Exp *new_key_value_list_exp(ast_unit *ast, Exp *values, int keyCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = keyValueListExp;
	e->keyValueList.keyValues = values;
	e->keyValueList.keyCount = keyCount;

	return e;
}

Exp *new_struct_exp(ast_unit *ast, Exp *type, Exp *list) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = structValueExp;
	e->structValue.type = type;
	e->structValue.list = list;

	return e;
}

Exp *new_array_exp(ast_unit *ast, Exp *values, int valueCount) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = arrayExp;
	e->array.values = values;
	e->array.valueCount = valueCount;

	return e;
}

Exp *new_array_type_exp(ast_unit *ast, Exp *type, Exp *length) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = arrayTypeExp;
	e->arrayType.type = type;
	e->arrayType.length = length;

	return e;
}

Exp *new_feild_type_exp(ast_unit *ast, Exp *type, Exp *name) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = fieldTypeExp;
	e->fieldType.type = type;
	e->fieldType.name = name;

	return e;
}

Exp *new_struct_type_exp(ast_unit *ast, Exp *fields, int count) {
	Exp *e = pool_get(ast->exp_pool);
	e->type = structTypeExp;
	e->structType.fields = fields;
	e->structType.feildCount = count;

	return e;
}

// TODO: consider remove this
Exp *newIntLiteral(ast_unit *ast, char *value) {
	Token t = {INT, 0, 0, value};
	return new_literal_exp(ast, t);
}

Smt *new_declare_smt(ast_unit *ast, Dcl *dcl) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = declareSmt;
	s->declare = dcl;

	return s; 
}

Smt *new_assignment_smt(ast_unit *ast, Exp *left, Exp *right) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = assignmentSmt;
	s->assignment.left = left;
	s->assignment.right = right;

	return s;
}

Smt *new_binary_assignment_smt(ast_unit *ast, Exp *left, TokenType op, Exp *right) {
	Smt *e = pool_get(ast->smt_pool);
	e->type = assignmentSmt;
	e->assignment.left = left;
	
	Token t = {ADD, 0, 0, ""};
	
	switch(op) {
		case ASSIGN:
			e->assignment.right = right;
			return e;
		case ADD_ASSIGN:
			t.type = ADD;
			break;
		case SUB_ASSIGN:
			t.type = SUB; 
			break;
		case MUL_ASSIGN:
			t.type = MUL;
			break;
		case REM_ASSIGN:
			t.type = REM;
			break;
		case OR_ASSIGN:
			t.type = OR; 
			break;
		case SHL_ASSIGN:
			t.type = SHL;
			break;
		default:
			ASSERT(false, "Expected an assignment token");
	}

	e->assignment.right = new_binary_exp(ast, left, t, right);
	return e; 
}

Smt *new_ret_smt(ast_unit *ast, Exp *result) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = returnSmt;
	s->ret.result = result;
	
	return s;
}

Smt *new_block_smt(ast_unit *ast, Smt *smts, int smtCount) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = blockSmt;
	s->block.smts = smts;
	s->block.count = smtCount;

	return s; 
}

Smt *new_if_smt(ast_unit *ast, Exp *cond, Smt *body, Smt *elses) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = ifSmt;
	s->ifs.cond = cond;
	s->ifs.body = body;
	s->ifs.elses = elses;

	return s;
}

Smt *new_for_smt(ast_unit *ast, Dcl *index, Exp *cond, Smt *inc, Smt *body) {
	Smt *s = pool_get(ast->smt_pool);
	s->type = forSmt;
	s->fors.index = index;
	s->fors.cond = cond;
	s->fors.inc = inc;
	s->fors.body = body;

	return s;
}

Dcl *new_varible_dcl(ast_unit *ast, char *name, Exp *type, Exp *value) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = varibleDcl;
	d->varible.name = name;
	d->varible.type = type;
	d->varible.value = value;

	return d;
}

Dcl *new_argument_dcl(ast_unit *ast, Exp *type, char *name) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = argumentDcl;
	d->argument.type = type;
	d->argument.name = name;

	return d;
}

Dcl *new_function_dcl(ast_unit *ast, char *name, Dcl *args, int argCount, Exp *returnType, Smt *body) {
	Dcl *d = pool_get(ast->dcl_pool);
	d->type = functionDcl;
	d->function.name = name;
	d->function.args = args;
	d->function.argCount = argCount;
	d->function.returnType = returnType;
	d->function.body = body;

	return d;
}
