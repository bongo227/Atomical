TEST(ParserTest, ScopeEnter) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    ASSERT_EQ(outer->outer, NULL);
    parser_enter_scope(p);
    ASSERT_TRUE(outer == p->scope->outer);
}

TEST(ParserTest, ScopeExit) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    parser_enter_scope(p);
    parser_exit_scope(p);
    ASSERT_TRUE(outer == p->scope);
}

TEST(ParserTest, ScopeInsert) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = new_argument_dcl(p->ast, NULL, (char *)"test_name");
    bool inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_TRUE(inserted);

    scope_object *found;
    HASH_FIND_STR((scope_object *)p->scope->objects, (char *)"test", found);
    ASSERT_STREQ(obj->name, found->obj->name);
    ASSERT_STREQ(obj->node->argument.name, 
        (char *)found->obj->node->argument.name);			

    inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_FALSE(inserted);
}

TEST(ParserTest, ScopeFind) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = NULL;
    parser_insert_scope(p, (char *)"test", obj);

    // Enter and exit some scopes
    parser_enter_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);

    Object *found = parser_find_scope(p, (char *)"test");
    ASSERT_EQ(obj->name, found->name); // pointer should be same

    found = parser_find_scope(p, (char *)"not here");
    ASSERT_EQ(found, NULL);
}

TEST(ParserTest, ParseLiteralExpression) {
    Exp *exp = parse_expression_from_string((char *)"123");

    ASSERT_FALSE(exp == NULL);
    ASSERT_EQ((int)literalExp, (int)exp->type);
    ASSERT_STREQ("123", exp->literal.value);
}

TEST(ParserTest, ParseIdentExpression) {
    Exp *exp = parse_expression_from_string((char *)"test");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test", exp->ident.name);
}

TEST(ParserTest, ParseIdentExpressionWithNumber) {
    Exp *exp = parse_expression_from_string((char *)"test123");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test123", exp->ident.name);
}

TEST(ParserTest, ParseBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
}

TEST(ParserTest, ParseBidmasBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b * c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)ADD, (int)exp->binary.op.type);
    ASSERT_EQ((int)MUL, (int)exp->binary.right->binary.op.type);
}

TEST(ParserTest, ParseSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.selector->ident.name);
}

TEST(ParserTest, ParseDoubleSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b.c");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_EQ((int)selectorExp, (int)exp->selector.exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.exp->selector.selector->ident.name);
    ASSERT_STREQ("c", exp->selector.selector->ident.name);
}

TEST(ParserTest, ParseIndexExpression) {
    Exp *exp = parse_expression_from_string((char *)"test[1]");

    ASSERT_EQ((int)indexExp, (int)exp->type);
}

TEST(ParserTest, ParseRightAssociativeBinaryOperators) {
    Exp *exp = parse_expression_from_string((char *)"a || b || c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)binaryExp, (int)exp->binary.right->type);
}

TEST(ParserTest, ParseUnaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"!a");
    
    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseUnaryMinuxExpression) {
    Exp *exp = parse_expression_from_string((char *)"-a");

    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseAssignmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a = b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}

TEST(ParserTest, ParseAddAssigmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a += b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
    ASSERT_EQ((int)binaryExp, (int)smt->assignment.right->type);
    ASSERT_EQ((int)ADD, (int)smt->assignment.right->binary.op.type);
    ASSERT_STREQ((char *)"a", smt->assignment.right->binary.left->ident.name);
    ASSERT_STREQ((char *)"b", smt->assignment.right->binary.right->ident.name);
}

TEST(ParserTest, ParseReturnStatment) {
    Smt *smt = parse_statement_from_string((char *)"return a");

    ASSERT_EQ((int)returnSmt, (int)smt->type);
}

TEST(ParserTest, ParseBlockStatment) {
    Smt *smt = parse_statement_from_string((char *)"{\nreturn test\n}");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);
}

TEST(ParserTest, ParserBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);    
}

TEST(ParserTest, ParserLongBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ a = 1; b = 2; return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(3, smt->block.count);
}

TEST(ParserTest, ParseIfStatment) {
    Smt *smt = parse_statement_from_string((char *)"if true {\nreturn false\n}");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_EQ(smt->ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseStatement) {
    Smt *smt = parse_statement_from_string((char *)"if true { return 123 } else { return 321 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elses = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseIfElseStatment) {
    Smt *smt = parse_statement_from_string((char *)"if false { return 321 } else if true { return 123 } else { return 0 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elif = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elif->type);
    ASSERT_NE(elif->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elif->ifs.body->type);
    ASSERT_NE(elif->ifs.elses, NULL);

    Smt *elses = elif->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}

TEST(ParserTest, ParserShortVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"a := 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_TRUE(obj->node == smt->declare);
}

TEST(ParserTest, ParseLongVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"var int a = 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);
    ASSERT_STREQ("a", smt->declare->varible.name);
    ASSERT_EQ((int)identExp, (int)smt->declare->varible.type->type);
    ASSERT_STREQ("int", smt->declare->varible.type->ident.name);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_NE(obj, NULL);
    ASSERT_TRUE(obj->node == smt->declare);
}

TEST(ParserTest, ParseArrayType) {
    const char *src = "int[3]";
    parser *p = new_parser(Lex((char *)src));
    Exp *type = parse_type(p);

    ASSERT_EQ((int)arrayTypeExp, (int)type->type);
}

TEST(ParserTest, ParseFunctionDefinition) {
    const char *src = "proc test :: int a, int b -> int {\nreturn a + b\n}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ((int)functionDcl, (int)dcl->type);
    ASSERT_EQ(2, (int)dcl->function.argCount);
    ASSERT_EQ((int)identExp, (int)dcl->function.returnType->type);

    Object *obja = parser_find_scope(p, (char *)"a");
    Object *objb = parser_find_scope(p, (char *)"b");
    
    ASSERT_NE(obja, NULL);
    ASSERT_NE(objb, NULL);
    ASSERT_NE(obja->node, objb->node);
    ASSERT_EQ(dcl->function.args, obja->node);
    ASSERT_EQ(dcl->function.args + 1, objb->node);
}

TEST(ParserTest, ParseEmptyCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test()");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(0, exp->call.argCount);
}

TEST(ParserTest, ParseCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(1, test)");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(2, exp->call.argCount);

    ASSERT_STREQ("1", exp->call.args[0].literal.value);
}

TEST(ParserTest, ParseCallInCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(test())");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(1, exp->call.argCount);
}

TEST(ParserTest, ParseForLoop) {
    Smt *smt = parse_statement_from_string((char *)"for i := 0; i < 10; i += 1 {}");

    ASSERT_EQ((int)forSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->fors.index->type);
    ASSERT_EQ((int)binaryExp, (int)smt->fors.cond->type);
    ASSERT_EQ((int)assignmentSmt, (int)smt->fors.inc->type);
    ASSERT_EQ((int)blockSmt, (int)smt->fors.body->type);
}

TEST(ParserTest, ParseIncrement) {
    Smt *smt = parse_statement_from_string((char *)"i++");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}

TEST(ParserTest, ParseKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{a: 1, b: 2}");

    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(2, exp->keyValueList.keyCount);
    ASSERT_STREQ("a", exp->keyValueList.keyValues[0].keyValue.key->ident.name);
    ASSERT_STREQ("b", exp->keyValueList.keyValues[1].keyValue.key->ident.name);
}

TEST(ParserTest, ParseEmptyKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(0, exp->keyValueList.keyCount);
}

TEST(ParserTest, ParseNullKeyValueList) {   
    Exp *exp = parse_expression_from_string((char *)"{1, 2, 3}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(3, exp->keyValueList.keyCount);
}

TEST(ParserTest, ParseArrayExpression) {
    Exp *exp = parse_expression_from_string((char *)"[1, 2, 3]");

    ASSERT_EQ((int)arrayExp, (int)exp->type);
    ASSERT_EQ(3, exp->array.valueCount);
}

TEST(ParserTest, ParseFunctionDclWithoutProc) {
    parser *p = new_parser(Lex((char *)"add :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, p->error_queue->queue_size);
    
    parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(PROC, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutName) {
    parser *p = new_parser(Lex((char *)"proc :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, p->error_queue->queue_size);
    
    parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutArgumentSeperator) {
    parser *p = new_parser(Lex((char *)"proc add -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, p->error_queue->queue_size);
    
    parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(DOUBLE_COLON, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutCommas) {
    parser *p = new_parser(Lex((char *)"proc add :: int a int b int c -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(2, p->error_queue->queue_size);
    
    for (int i = 0; i < 2; i++) {
        parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
        ASSERT_EQ(parser_error_expect_token, error->type);
        ASSERT_EQ(1, error->length);
        ASSERT_EQ(COMMA, error->expect_token.type);
    }
}

TEST(ParserTest, ParseFunctionDclWithoutArgTypeOrName) {
    parser *p = new_parser(Lex((char *)"proc add :: int, int a -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, p->error_queue->queue_size);
    
    parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}

TEST(ParserTest, ParseFunctionDclWithoutArrow) {
    parser *p = new_parser(Lex((char *)"proc add :: int a {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, p->error_queue->queue_size);
    
    parser_error *error = (parser_error *)queue_dequeue(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ARROW, error->expect_token.type);
}