TEST(ParserTest, ScopeEnter) {
    Parser *parser = NewParser("", NULL);
    Scope *outer = parser->scope;
    ASSERT_EQ(outer->outer, NULL);
    EnterScope(parser);
    ASSERT_TRUE(outer == parser->scope->outer);
}

TEST(ParserTest, ScopeExit) {
    Parser *parser = NewParser("", NULL);
    Scope *outer = parser->scope;
    EnterScope(parser);
    ExitScope(parser);
    ASSERT_TRUE(outer == parser->scope);
}

TEST(ParserTest, ScopeInsert) {
    Parser *parser = NewParser("", NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = "test";
    obj->node = "nodevalue";
    obj->typeInfo = NULL;
    bool inserted = InsertScope(parser, "test", obj);
    ASSERT_TRUE(inserted);

    ScopeObject *found;
    HASH_FIND_STR(parser->scope->objects, "test", found);
    ASSERT_STREQ(obj->name, found->obj->name);
    ASSERT_STREQ((char *)obj->node, (char *)found->obj->node);			

    inserted = InsertScope(parser, "test", obj);
    ASSERT_FALSE(inserted);
}

TEST(ParserTest, ScopeFind) {
    Parser *parser = NewParser("", NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = "test";
    obj->node = NULL;
    obj->typeInfo = NULL;
    InsertScope(parser, "test", obj);

    // Enter and exit some scopes
    EnterScope(parser);
    EnterScope(parser);
    ExitScope(parser);
    EnterScope(parser);
    ExitScope(parser);

    Object *found = FindScope(parser, "test");
    ASSERT_EQ(obj->name, found->name); // pointer should be same

    found = FindScope(parser, "not here");
    ASSERT_EQ(found, NULL);
}

TEST(ParserTest, ParseLiteralExpression) {
    char *src = "123";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_FALSE(exp == NULL);
    ASSERT_EQ((int)literalExp, (int)exp->type);
    ASSERT_STREQ("123", exp->node.literal.value);
}

TEST(ParserTest, ParseIdentExpression) {
    char *src = "test";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test", exp->node.ident.name);
}

TEST(ParserTest, ParseIdentExpressionWithNumber) {
    char *src = "test123";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test123", exp->node.ident.name);
}

TEST(ParserTest, ParseBinaryExpression) {
    char *src = "a + b";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)binaryExp, (int)exp->type);
}

TEST(ParserTest, ParseBidmasBinaryExpression) {
    char *src = "a + b * c";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)ADD, (int)exp->node.binary.op.type);
    ASSERT_EQ((int)MUL, (int)exp->node.binary.right->node.binary.op.type);
}

TEST(ParserTest, ParseSelectorExpression) {
    char *src = "a.b";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_STREQ("a", exp->node.selector.exp->node.ident.name);
    ASSERT_STREQ("b", exp->node.selector.selector->node.ident.name);
}

TEST(ParserTest, ParseDoubleSelectorExpression) {
    char *src = "a.b.c";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_EQ((int)selectorExp, (int)exp->node.selector.exp->type);
    ASSERT_STREQ("a", exp->node.selector.exp->node.selector.exp->node.ident.name);
    ASSERT_STREQ("b", exp->node.selector.exp->node.selector.selector->node.ident.name);
    ASSERT_STREQ("c", exp->node.selector.selector->node.ident.name);
}

TEST(ParserTest, ParseIndexExpression) {
    char *src = "test[1]";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)indexExp, (int)exp->type);
}

TEST(ParserTest, ParseRightAssociativeBinaryOperators) {
    char *src = "a || b || c";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)binaryExp, (int)exp->node.binary.right->type);
}

TEST(ParserTest, ParseUnaryExpression) {
    char *src = "!a";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);
    
    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseUnaryMinuxExpression) {
    char *src = "-a";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)unaryExp, (int)exp->type);
}

TEST(ParserTest, ParseAssignmentOperator) {
    char *src = "a = b";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}

TEST(ParserTest, ParseAddAssigmentOperator) {
    char *src = "a += b";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
    ASSERT_EQ((int)binaryExp, (int)smt->node.assignment.right->type);
    ASSERT_EQ((int)ADD, (int)smt->node.assignment.right->node.binary.op.type);
    ASSERT_STREQ("a", smt->node.assignment.right->node.binary.left->node.ident.name);
    ASSERT_STREQ("b", smt->node.assignment.right->node.binary.right->node.ident.name);
}

TEST(ParserTest, ParseReturnStatment) {
    char *src = "return a";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)returnSmt, (int)smt->type);
}

TEST(ParserTest, ParseBlockStatment) {
    char *src = "{\nreturn test\n}";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->node.block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->node.block.smts->type);
}

TEST(ParserTest, ParserBlockSingleLine) {
    char *src = "{ return test }";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->node.block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->node.block.smts->type);    
}

TEST(ParserTest, ParserLongBlockSingleLine) {
    char *src = "{ a = 1; b = 2; return test }";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(3, smt->node.block.count);
}

TEST(ParserTest, ParseIfStatment) {
    char *src = "if true {\nreturn false\n}";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->node.ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->node.ifs.body->type);
    ASSERT_EQ(smt->node.ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseStatement) {
    char *src = "if true { return 123 } else { return 321 }";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->node.ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->node.ifs.body->type);
    ASSERT_NE(smt->node.ifs.elses, NULL);

    Smt *elses = smt->node.ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->node.ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->node.ifs.body->type);
    ASSERT_EQ(elses->node.ifs.elses, NULL);
}

TEST(ParserTest, ParseIfElseIfElseStatment) {
    char *src = "if false { return 321 } else if true { return 123 } else { return 0 }";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->node.ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->node.ifs.body->type);
    ASSERT_NE(smt->node.ifs.elses, NULL);

    Smt *elif = smt->node.ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elif->type);
    ASSERT_NE(elif->node.ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elif->node.ifs.body->type);
    ASSERT_NE(elif->node.ifs.elses, NULL);

    Smt *elses = elif->node.ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->node.ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->node.ifs.body->type);
    ASSERT_EQ(elses->node.ifs.elses, NULL);
}

TEST(ParserTest, ParserShortVaribleDeclare) {
    char *src = "a := 10";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->node.declare->type);

    Object *obj = FindScope(parser, "a");
    ASSERT_TRUE(obj->node == smt->node.declare);
}

TEST(ParserTest, ParseLongVaribleDeclare) {
    char *src = "var int a = 10";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->node.declare->type);
    ASSERT_EQ((int)identExp, (int)smt->node.declare->node.varible.name->type);
    ASSERT_STREQ("int", smt->node.declare->node.varible.type->node.ident.name);

    Object *obj = FindScope(parser, "a");
    ASSERT_NE(obj, NULL);
    ASSERT_TRUE(obj->node == smt->node.declare);
}

TEST(ParserTest, ParseArrayType) {
    char *src = "int[3]";
    Parser *parser = NewParser(src, Lex(src));
    Exp *type = ParseType(parser);

    ASSERT_EQ((int)arrayTypeExp, (int)type->type);
}

TEST(ParserTest, ParseFunctionDefinition) {
    char *src = "proc test :: int a, int b -> int {\nreturn a + b\n}";
    Parser *parser = NewParser(src, Lex(src));
    Dcl *dcl = ParseFunction(parser);

    ASSERT_EQ((int)functionDcl, (int)dcl->type);
    ASSERT_EQ(2, (int)dcl->node.function.argCount);
    ASSERT_EQ((int)identExp, (int)dcl->node.function.returnType->type);

    Object *obja = FindScope(parser, "a");
    Object *objb = FindScope(parser, "b");
    
    ASSERT_NE(obja, NULL);
    ASSERT_NE(objb, NULL);
    ASSERT_NE(obja->node, objb->node);
    ASSERT_EQ(dcl->node.function.args, obja->node);
    ASSERT_EQ(dcl->node.function.args + 1, objb->node);
}

TEST(ParserTest, ParseEmptyCallExpression) {
    char *src = "test()";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(0, exp->node.call.argCount);
}

TEST(ParserTest, ParseCallExpression) {
    char *src = "test(1, test)";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(2, exp->node.call.argCount);

    ASSERT_STREQ("1", exp->node.call.args[0].node.literal.value);
}

TEST(ParserTest, ParseCallInCallExpression) {
    char *src = "test(test())";
    Parser *parser = NewParser(src, Lex(src));
    Exp *exp = ParseExpression(parser, 0);

    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(1, exp->node.call.argCount);
}

TEST(ParserTest, ParseForLoop) {
    char *src = "for i := 0; i < 10; i += 1 {}";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)forSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->node.fors.index->type);
    ASSERT_EQ((int)binaryExp, (int)smt->node.fors.cond->type);
    ASSERT_EQ((int)assignmentSmt, (int)smt->node.fors.inc->type);
    ASSERT_EQ((int)blockSmt, (int)smt->node.fors.body->type);
}

TEST(ParserTest, ParseIncrement) {
    char *src = "i++";
    Parser *parser = NewParser(src, Lex(src));
    Smt *smt = ParseStatement(parser);

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}