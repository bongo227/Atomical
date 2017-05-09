## Testing
When creating this project I used the test driven development workflow in which you write a test first, watch it fail, then write the code to make it parse. By following this I have constructed a test program which tests every unit of the compiler as well as integration tests to test the whole system. This is so important when building software to ensure no new code breaks any features. 

### String test
#### StringTest.CreateNewString 
Tests that new strings, from a C string, are constructed correctly
```c
TEST(StringTest, CreateNewString) {
    string s = string_new("test");
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}
```

#### StringTest.CreateNewStringLength
Test that new strings, from a c string with a specified length, are constructed correctly
```c
TEST(StringTest, CreateNewStringLength) {
    string s = string_new_length("testtesttest", 4);
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}
```

#### StringTest.CreateNewStringFile
Test that new strings, from a file handle, are constructed correctly
```c
TEST(StringTest, CreateNewStringFile) {
    FILE *f = fopen("/tmp/string_test_file.fur", "w");
    fprintf(f, "test");
    fclose(f);

    f = fopen("/tmp/string_test_file.fur", "r");
    string s = string_new_file(f);
    fclose(f);

    ASSERT_EQ(0, strcmp(s, "test"));
}
```

#### Stringtest.CopyString
Test that a string is copyed correctly
```c
TEST(StringTest, CopyString) {
    string s = string_new("test");
    string copy = string_copy(s);
    ASSERT_NE(s, copy);
    ASSERT_EQ(0, strcmp(s, copy));
}
```

#### StringTest.StringLength
Test that the correct length of a string is returned
```c
TEST(StringTest, StringLength) {
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}
```

#### StringTest.StringCapacity
Test that a string returns the correct capacity
```c
TEST(StringTest, StringCapacity) { 
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}
```

#### StringTest.StringAvalivle
Test that string returns the correct amount of space avalible
```c
TEST(StringTest, StringAvalible) {
    string s = string_new("test");
    ASSERT_EQ(0, string_avalible(s));
}
```

#### StringTest.StringExpand
test that a string correctly expands its space
```c
TEST(StringTest, StringExpand) {
    string s = string_new("test");
    s = string_expand(s, 10);
    ASSERT_EQ(10, string_capacity(s));
    ASSERT_EQ(6, string_avalible(s));
    ASSERT_EQ(4, string_length(s));
    ASSERT_EQ(0, strcmp(s, "test"));
}
```

#### StringTest.StringClear
Test that a string correctly clears its data
```c
TEST(StringTest, StringClear) {
    string s = string_new("test");
    s = string_clear(s);
    ASSERT_EQ(0, strcmp(s, ""));
}
```

#### StringTest.StringAppend
Test that two strings are correctly appended
```c
TEST(StringTest, StringAppend){
    string s1 = string_new("hello ");
    string s2 = string_new("world");
    s1 = string_append(s1, s2);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringAppendLength
Test that two strings, with a specified length, are correctly appended
```c
TEST(StringTest, StringAppendLength) {
    string s1 = string_new("hello ");
    string s2 = string_new("world of earth");
    s1 = string_append_length(s1, s2, 5);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringAppendCString
Test that a string and a C String are appended correctly
```c
TEST(StringTest, StringAppendCString) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringMultipleAppendWithMalloc
Test that a string reallocates correctly with a memory allocation in the middle
```c
TEST(StringTest, StringMultipleAppendWithMalloc) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    malloc(10);
    s1 = string_append_cstring(s1, (char *)" of");
    s1 = string_append_cstring(s1, (char *)" ours");

    ASSERT_EQ(0, strcmp(s1, "hello world of ours"));
}
```

#### StringTest.StringSliceEnd
Test that a string is correctly sliced from the end
```c
TEST(StringTest, StringSliceEnd) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 0, 11);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringSlice
Test that a string is correctly sliced in the middle
```c
TEST(StringTest, StringSlice) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 6, 11);
    ASSERT_EQ(0, strcmp(s1, "world"));
}
```

#### StringTest.StringEqual
Test that two equal strings are compared correctly
```c
TEST(StringTest, StringEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar");
    ASSERT_EQ(true, string_equals(s1, s2));
}
```

#### StringTest.StringNotEqualLength
Test that two non equal lengthed strings are compared correctly
```c
TEST(StringTest, StringNotEqualsLength) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}
```

#### StringTest.StringNotEqual
Test that tow equal lengthed strings are compared correctly
```c
TEST(StringTest, StringNotEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}
```

### Pool test

#### PoolTest.NewPool
Tests that a new pool is correctly constructed
```c
TEST(PoolTest, NewPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.InsertPool
Tests that an item is correctly inserted into the pool
```c
TEST(PoolTest, InsertPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int v1 = 123;
    int *e1 = (int *)pool_get(int_pool);
    memcpy(e1, &v1, sizeof(int));

    ASSERT_EQ(v1, *e1);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(1, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.FillPool
Tests the pool can be filled and `pool_full()` returns `true`
```c
TEST(PoolTest, FillPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 5; i++) {
        int *e = (int *)pool_get(int_pool);
        memcpy(e, &i, sizeof(int));
    }

    ASSERT_TRUE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(5, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.ExtendPool
Tests the pool capacity can be extended
```c
TEST(PoolTest, ExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_extend(int_pool, 10);
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### Pooltest.OverflowExtendPool
Tests inserting into a full pool extends the pool
```c
TEST(PoolTest, OverflowExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 6; i++) {
        int *pool_element = (int *)pool_get(int_pool);
        memcpy(pool_element, &i, sizeof(int));
    }

    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(6, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.ReleaseElementFromPool
Tests an element is correctly released from the pool 
```c
TEST(PoolTest, ReleaseElementFromPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));
    
    ASSERT_EQ(3, pool_count(int_pool));
    
    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));
    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);

    pool_destroy(int_pool);
}
```

#### PoolTest.ReusePool
Tests that memory can be reclaimed and reused from the pool
```c
TEST(PoolTest, ReusePool) {
    pool *int_pool = new_pool(sizeof(int), 3);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));

    ASSERT_EQ(3, pool_count(int_pool));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(3, *e3);

    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));

    int *e4 = (int *)pool_get(int_pool);
    ASSERT_EQ(*e3, *e4);
    
    int v4 = 4;
    memcpy(e4, &v4, sizeof(int));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(4, *e4);
}
```

### Queue test
#### QueueTest.NewQueue
Tests that a new queue is correctly constructed
```c
TEST(QueueTest, NewQueue) {
    queue *q = new_queue(sizeof(int));
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(sizeof(int) + sizeof(queue_item), q->element_size);
}
```

#### QueueTest.PushPopFrount
Tests that an element can be correctly pushed and popped from the frount
```c
TEST(QueueTest, PushPopFront) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_front(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_front(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}
```

#### QueueTest.PushPopBack
Tests that an element can be correctly pushed and popped from the back
```c
TEST(QueueTest, PushPopBack) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_back(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_back(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}
```

#### QueueTest.PushPopManyFrount
Tests that many elements can be pushed and popped off the front in the correct order
```c
TEST(QueueTest, PushPopManyFront) {
    queue *q = new_queue(sizeof(int));
    for(int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_back(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 0; i < 100; i++) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
```

#### QueueTest.PushPopManyBack
Tests that many elements can be pushed and popped off the back in the correct order
```c
TEST(QueueTest, PushPopManyBack) {
    queue *q = new_queue(sizeof(int));
    for (int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_front(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 99; i >= 0; i--) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
```

### Lexer test

#### LexerTest.Identifier
Test that identifiers are lexed correctly
```c
TEST(LexerTest, Identifier) {

    tcase cases[] = {
        tcase{"test", IDENT, "test"},
        tcase{"a", IDENT, "a" },
        tcase{"test123", IDENT, "test123"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Numbers
Test that integer numbers are lexed correctly
```c
TEST(LexerTest, Numbers) {
    tcase cases[] = {
        tcase{ "1", INT, "1" },
        tcase{ "1204", INT, "1204" },

        tcase{ "213.42", FLOAT, "213.42"},
        tcase{ "0.5", FLOAT, ".5" },
        
        tcase{"0x1000", HEX, "1000"},
        tcase{"0600", OCTAL, "600"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Strings
Test that strings are lexed correctly
```c
TEST(LexerTest, Strings) {
    tcase cases[] = {
        tcase{ "\"test\"", STRING, "test" },
        tcase{ "\"\"", STRING, "" },

        tcase{ "\"\n\"", STRING, "\n" },
        tcase{ "\"\021\"", STRING, "\021" },
        tcase{ "\"\x41\"", STRING, "\x41" },
        tcase{ "\"\u1000\"", STRING, "\u1000" },
        tcase{ "\"\u10001000\"", STRING, "\u10001000" },
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Symbols
Tests that all symbols are lexed correctly
```c
TEST(LexerTest, Symbols) {
    tcase cases[] = {
        tcase{ ":", COLON, "" },
        tcase{ ":=", DEFINE, "" },
        tcase{ "::", DOUBLE_COLON, "" },

        tcase{ ".", PERIOD, "" },
        tcase{ "...", ELLIPSE, "" },

        tcase{ ",", COMMA, "" },

        tcase{ "(", LPAREN, "" },
        tcase{ ")", RPAREN, "" },
        tcase{ "[", LBRACK, "" },
        tcase{ "]", RBRACK, "" },
        tcase{ "{", LBRACE, "" },
        tcase{ "}", RBRACE, "" },

        tcase{ "+", ADD, "" },
        tcase{ "+=", ADD_ASSIGN, "" },
        tcase{ "++", INC, "" },

        tcase{ "-", SUB, "" },
        tcase{ "-=", SUB_ASSIGN, "" },
        tcase{ "--", DEC, "" },
        tcase{ "->", ARROW, "" },

        tcase{ "*", MUL, "" },
        tcase{ "*=", MUL_ASSIGN, "" },

        tcase{ "/", QUO, "" },
        tcase{ "/=", QUO_ASSIGN, "" },

        tcase{ "%", REM, "" },
        tcase{ "%=", REM_ASSIGN, "" },

        tcase{ "^", XOR, "" },
        tcase{ "^=", XOR_ASSIGN, "" },

        tcase{ "<", LSS, "" },
        tcase{ "<=", LEQ, "" },
        tcase{ "<<", SHL, "" },
        tcase{ "<<=", SHL_ASSIGN, "" },

        tcase{ ">", GTR, "" },
        tcase{ ">=", GEQ, "" },
        tcase{ ">>", SHR, "" },
        tcase{ ">>=", SHR_ASSIGN, "" },

        tcase{ "=", ASSIGN, "" },
        tcase{ "==", EQL, "" },

        tcase{ "!", NOT, "" },
        tcase{ "!=", NEQ, "" },

        tcase{ "&", AND, "" },
        tcase{ "&=", AND_ASSIGN, "" },
        tcase{ "&&", LAND, "" },
        tcase{ "&^", AND_NOT, "" },
        tcase{ "&^=", AND_NOT_ASSIGN, "" },
    
        tcase{"|", OR, ""},
        tcase{"||", LOR, ""},
        tcase{"|=", OR_ASSIGN, ""},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.LineNumbers
Tests the line numbers of the tokens are correct
```c
TEST(LexerTest, LineNumbers) {
    Token *tokens = Lex((char *)"1\n2\n3");
    
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(i+1, tokens[i].line);	
    }
}
```

#### LexerTest.ColumnNumbers
Tests the column numbers of the tokens are correct
```c
TEST(LexerTest, ColumnNumbers) {
    Token *tokens = Lex((char *)"foo bar baz");

    ASSERT_EQ(1, tokens[0].column);
    ASSERT_EQ(5, tokens[1].column);
    ASSERT_EQ(9, tokens[2].column);
}
```

#### LexerTest.SemiColonInsertion
Tests that semi colons are inserted correctly
```c
TEST(LexerTest, SemiColonInsertion) {
    Token *tokens = Lex((char *)"foo\nbar");
    ASSERT_STREQ(TokenName(SEMI), TokenName(tokens[1].type));
}
```

### Parser test

#### ParserTest.ScopeEnter
Test that a new scope is correctly entered
```c
TEST(ParserTest, ScopeEnter) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    ASSERT_EQ(outer->outer, NULL);
    parser_enter_scope(p);
    ASSERT_TRUE(outer == p->scope->outer);
}
```

#### ParserTest.ScopeExit
Tests that a scope is correctly exited
```c
TEST(ParserTest, ScopeExit) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    parser_enter_scope(p);
    parser_exit_scope(p);
    ASSERT_TRUE(outer == p->scope);
}
```

#### ParserTest.ScopeInsert
Tests that a new object is correctly inserted into the scope
```c
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
```

#### ParserTest.ScopeFind
Tests that an object can be found in scope
```c
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
```

#### ParserTest.ParseLiteralExpression
Tests that a literal expression is parsed correctly
```c
TEST(ParserTest, ParseLiteralExpression) {
    Exp *exp = parse_expression_from_string((char *)"123");

    ASSERT_FALSE(exp == NULL);
    ASSERT_EQ((int)literalExp, (int)exp->type);
    ASSERT_STREQ("123", exp->literal.value);
}
```

#### ParserTest.ParseIdentExpression
Tests that a identifier expression is parsed correcly
```c
TEST(ParserTest, ParseIdentExpression) {
    Exp *exp = parse_expression_from_string((char *)"test");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test", exp->ident.name);
}
```

#### ParserTest.ParseIdentExpressionWithNumber
Tests that a identifier expression, with a number, is parsed correctly
```c
TEST(ParserTest, ParseIdentExpressionWithNumber) {
    Exp *exp = parse_expression_from_string((char *)"test123");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test123", exp->ident.name);
}
```

#### ParserTest.ParseBinaryExpression
Tests that a binary expression is parsed correctly
```c
TEST(ParserTest, ParseBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
}
```

#### ParserTest.ParseBidmasBinaryExpression
Tests that the rules of BIDMAS when parsing binary expressions are followed
```c
TEST(ParserTest, ParseBidmasBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b * c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)ADD, (int)exp->binary.op.type);
    ASSERT_EQ((int)MUL, (int)exp->binary.right->binary.op.type);
}
```

#### ParserTest.ParseSelectorExpression
Tests that a selector expression is correctly parsed
```c
TEST(ParserTest, ParseSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.selector->ident.name);
}
```

#### ParserTest.ParseDoubleSelectorExpression
Tests that a double selector expression is correclty parsed
```c
TEST(ParserTest, ParseDoubleSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b.c");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_EQ((int)selectorExp, (int)exp->selector.exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.exp->selector.selector->ident.name);
    ASSERT_STREQ("c", exp->selector.selector->ident.name);
}
```

#### ParserTest.ParseIndexExpression
Tests that an index expression is correctly parsed
```c
TEST(ParserTest, ParseIndexExpression) {
    Exp *exp = parse_expression_from_string((char *)"test[1]");

    ASSERT_EQ((int)indexExp, (int)exp->type);
}
```

#### ParserTest.ParserightAssociativeBinaryOperators
Tests that right associative binary operators are parsed correctly
```c
TEST(ParserTest, ParseRightAssociativeBinaryOperators) {
    Exp *exp = parse_expression_from_string((char *)"a || b || c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)binaryExp, (int)exp->binary.right->type);
}
```

#### ParserTest.ParseUnaryExpression
Tests that a unary expression is parsed correctly
```c
TEST(ParserTest, ParseUnaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"!a");
    
    ASSERT_EQ((int)unaryExp, (int)exp->type);
}
```

#### ParserTest.ParseUnaryMinusExpression
Tests that a unary minus expression works correctly
```c
TEST(ParserTest, ParseUnaryMinusExpression) {
    Exp *exp = parse_expression_from_string((char *)"-a");

    ASSERT_EQ((int)unaryExp, (int)exp->type);
}
```

#### ParserTest.ParseAssignmentOperator
Tests that a assignment expression works correctly
```c
TEST(ParserTest, ParseAssignmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a = b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}
```

#### ParserTest.ParseAddAssigmentOperator
Tests that a add assignment expression is parsed correctly
```c
TEST(ParserTest, ParseAddAssigmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a += b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
    ASSERT_EQ((int)binaryExp, (int)smt->assignment.right->type);
    ASSERT_EQ((int)ADD, (int)smt->assignment.right->binary.op.type);
    ASSERT_STREQ((char *)"a", smt->assignment.right->binary.left->ident.name);
    ASSERT_STREQ((char *)"b", smt->assignment.right->binary.right->ident.name);
}
```

#### ParserTest.ParseReturnStatment
Tests that a return statement is parsed correctly
```c
TEST(ParserTest, ParseReturnStatment) {
    Smt *smt = parse_statement_from_string((char *)"return a");

    ASSERT_EQ((int)returnSmt, (int)smt->type);
}
```

#### ParserTest.ParseBlockStatment
Tests that a block statement is parsed correctly
```c
TEST(ParserTest, ParseBlockStatment) {
    Smt *smt = parse_statement_from_string((char *)"{\nreturn test\n}");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);
}
```

#### ParserTest.ParseBlockSingleLine
Tests that a single line block is parsed correctly
```c
TEST(ParserTest, ParserBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);    
}
```

#### ParserTest.ParseLongBlockSingleLine
Tests that a single line block with many statements is parsed correctly
```c
TEST(ParserTest, ParserLongBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ a = 1; b = 2; return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(3, smt->block.count);
}
```

#### ParserTest.ParseIfStatment
Tests that an if statement is parsed correctly
```c
TEST(ParserTest, ParseIfStatment) {
    Smt *smt = parse_statement_from_string((char *)"if true {\nreturn false\n}");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_EQ(smt->ifs.elses, NULL);
}
```

#### ParserTest.ParseIfElseStatment
Tests that an if else statement is parsed correctly
```c
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
```

#### ParserTest.ParseIfElseIfElseStatment
Tests that an if else if else statement is parsed correctly
```c
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
```

#### ParserTest.ParseShortVaribleDeclare
Tests that a short variable declaration is parsed correctly
```c
TEST(ParserTest, ParserShortVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"a := 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_TRUE(obj->node == smt->declare);
}
```

#### ParserTest.ParseLongVaribleDeclare
Tests that a long variable declaration is parsed correctly
```c
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
```

#### ParserTest.ParseArrayType
Tests that an array type is parsed correctly
```c
TEST(ParserTest, ParseArrayType) {
    const char *src = "int[3]";
    parser *p = new_parser(Lex((char *)src));
    Exp *type = parse_type(p);

    ASSERT_EQ((int)arrayTypeExp, (int)type->type);
}
```

#### ParserTest.ParseFunctionDefinition
Tests that a function definition is parsed correctly
```c
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
```

#### ParserTest.ParseEmptyCallExpression
Tests that a call expression is parsed correctly
```c
TEST(ParserTest, ParseEmptyCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test()");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(0, exp->call.argCount);
}
```

#### ParserTest.ParseCallExpression
Tests that a call expression is parsed correctly
```c
TEST(ParserTest, ParseCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(1, test)");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(2, exp->call.argCount);

    ASSERT_STREQ("1", exp->call.args[0].literal.value);
}
```

#### ParserTest.ParseCallInCallExpression
Tests that a call in a call expression is parsed correctly
```c
TEST(ParserTest, ParseCallInCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(test())");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(1, exp->call.argCount);
}
```

#### ParserTest.ParseForLoop
Tests that a for loop is parsed correctly
```c
TEST(ParserTest, ParseForLoop) {
    Smt *smt = parse_statement_from_string((char *)"for i := 0; i < 10; i += 1 {}");

    ASSERT_EQ((int)forSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->fors.index->type);
    ASSERT_EQ((int)binaryExp, (int)smt->fors.cond->type);
    ASSERT_EQ((int)assignmentSmt, (int)smt->fors.inc->type);
    ASSERT_EQ((int)blockSmt, (int)smt->fors.body->type);
}
```

#### ParserTest.ParseIncrement
Tests that a increment statement is parsed correctly
```c
TEST(ParserTest, ParseIncrement) {
    Smt *smt = parse_statement_from_string((char *)"i++");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}
```

#### ParserTest.ParseKeyValueList
Tests that a key value list is parsed correctly
```c
TEST(ParserTest, ParseKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{a: 1, b: 2}");

    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(2, exp->keyValueList.keyCount);
    ASSERT_STREQ("a", exp->keyValueList.keyValues[0].keyValue.key->ident.name);
    ASSERT_STREQ("b", exp->keyValueList.keyValues[1].keyValue.key->ident.name);
}
```

#### ParserTest.ParseEmptyKeyValueList
Tests that an empty key value list is parsed correctly
```c
TEST(ParserTest, ParseEmptyKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(0, exp->keyValueList.keyCount);
}
```

#### ParserTest.ParseNullKeyValueList
Tests that an key value list with null keys is parsed correctly
```c
TEST(ParserTest, ParseNullKeyValueList) {   
    Exp *exp = parse_expression_from_string((char *)"{1, 2, 3}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(3, exp->keyValueList.keyCount);
}
```

#### ParserTest.ParseArrayExpression
Tests that an array initilizer expression is parsed correctly
```c
TEST(ParserTest, ParseArrayExpression) {
    Exp *exp = parse_expression_from_string((char *)"[1, 2, 3]");

    ASSERT_EQ((int)arrayExp, (int)exp->type);
    ASSERT_EQ(3, exp->array.valueCount);
}
```

#### ParserTest.ParseFunctionDclWithoutProc
Tests that a function declaration is parsed without a proc (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutProc) {
    parser *p = new_parser(Lex((char *)"add :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(PROC, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutName
Tests that a function declaration is parsed without a name (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutName) {
    parser *p = new_parser(Lex((char *)"proc :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutArgumentSeperator
Tests that a function declaration without an argument seperator (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutArgumentSeperator) {
    parser *p = new_parser(Lex((char *)"proc add -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(DOUBLE_COLON, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutCommas
Tests that a function declaration without commas are passed correctly (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutCommas) {
    parser *p = new_parser(Lex((char *)"proc add :: int a int b int c -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(2, queue_size(p->error_queue));
    
    for (int i = 0; i < 2; i++) {
        parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
        ASSERT_EQ(parser_error_expect_token, error->type);
        ASSERT_EQ(1, error->length);
        ASSERT_EQ(COMMA, error->expect_token.type);
    }
}
```

#### ParserTest.ParseFunctionDclWithoutArgTypeOrName
Tests that a function declaration without argument type or name is parsed correctly (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutArgTypeOrName) {
    parser *p = new_parser(Lex((char *)"proc add :: int, int a -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutArrow
Tests that a function declaration without an arrow is parsed correctly (and error is produced)
```c
TEST(ParserTest, ParseFunctionDclWithoutArrow) {
    parser *p = new_parser(Lex((char *)"proc add :: int a {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ARROW, error->expect_token.type);
}
```

#### ParserTest.ParseNonDeclaration
Tests that an error is produced when a declaration is expected
```c
TEST(ParserTest, ParseNonDecleration) {
    parser *p = new_parser(Lex((char *)"return a"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_declaration, error->type);
    ASSERT_EQ(1, error->length);
}
```

#### ParserTest.ParseVaribleDclWithoutType
Tests that a long variable declaration without a type is parsed correclty (and error is produced)
```c
TEST(ParserTest, ParseVaribleDclWithoutType) {
    parser *p = new_parser(Lex((char *)"var = 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_type, error->type);
    ASSERT_EQ(1, error->length);
}
```

#### ParserTest.ParseVariableDclWithoutName
Tests that an error is produced when a varible declaration doesnt have a name
```c
TEST(ParserTest, ParseVariableDclWithoutName) {
    parser *p = new_parser(Lex((char *)"var int = 100"));
    Dcl *dcl = parse_declaration(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseVaribleDclWithoutEquals
Tests that a long variable declaration without an equals is parsed correctly (and error is produced)
```c
TEST(ParserTest, ParseVaribleDclWithoutEquals) {
    parser *p = new_parser(Lex((char *)"var int a 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ASSIGN, error->expect_token.type);
}
```

#### ParserTest.ParseVaribleDclWithoutValue
Tests that a long variable declaration without a value produces error
```c
TEST(ParserTest, ParseVaribleDclWithoutValue) {
    parser *p = new_parser(Lex((char *)"var int a = "));
    Dcl *dcl = parse_declaration(p);
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_prefix, error->type);
    ASSERT_EQ(1, error->length);
}
```

### Irgen tests
```
#define TEST_TYPE(name, src, expectedType) TEST(IrgenTest, name){       \
    parser *p = new_parser(Lex((char *)src));                           \
    Exp *e = parse_type(p);                                             \
    Irgen *irgen = NewIrgen();                                          \
    LLVMTypeRef type = CompileType(e);                                  \
    ASSERT_TRUE(type == expectedType);                                  \
}        

#define TEST_LITERAL(name, src, expectedType, expectedValue) TEST(IrgenTest, name) {    \
    parser *p = new_parser(Lex((char *)src));                                           \
    Exp *e = parse_expression(p, 0);                                                    \
    Irgen *irgen = NewIrgen();                                                          \
    LLVMValueRef value = CompileLiteralExp(irgen, e);                                   \
    ASSERT_TRUE(LLVMIsConstant(value));                                                 \
    ASSERT_STREQ(expectedValue, LLVMPrintValueToString(value));                         \
}

#define TEST_CAST(name, value, cast) TEST(IrgenTest, name) {                        \
    Irgen *irgen = NewIrgen();                                                      \
    irgen->function = LLVMAddFunction(irgen->module, "test", LLVMInt64Type());      \
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");       \
    irgen->builder = LLVMCreateBuilder();                                           \
    LLVMPositionBuilderAtEnd(irgen->builder, entry);                                \
    LLVMValueRef castValue = Cast(irgen, value, cast);                              \
    ASSERT_EQ(LLVMTypeOf(castValue), cast);                                         \
} 
```

#### IrgenTest.CompileTypeInt
Tests integer type is compiled correctly
```c
TEST_TYPE(CompileTypeInt, "int", LLVMInt64Type())
```

#### IrgenTest.CompileTypeI64
Tests 64 bit integer type is compiled correctly
```c
TEST_TYPE(CompileTypeI64, "i64", LLVMInt64Type())
```

#### IrgenTest.CompileTypeI32
Tests 32 bit integer type is compiled correctly
```c
TEST_TYPE(CompileTypeI32, "i32", LLVMInt32Type())
```

#### IrgenTest.CompileTypeI16
Tests 16 bit integer type is compiled correctly
```c
TEST_TYPE(CompileTypeI16, "i16", LLVMInt16Type())
```

#### IrgenTest.CompileTypeI8
Tests 8 bit integer type is compiled correctly
```c
TEST_TYPE(CompileTypeI8, "i8", LLVMInt8Type())
```

#### IrgenTest.CompileTypeFloat
Tests float type is compiled correctly
```c
TEST_TYPE(CompileTypeFloat, "float", LLVMFloatType())
```

#### IrgenTest.CompileTypeF64
Tests 64 bit float type is compiled correctly
```c
TEST_TYPE(CompileTypeF64, "f64", LLVMDoubleType())
```

#### IrgenTest.CompileTypeF32
Tests 32 bit float type is compiled correctly
```c
TEST_TYPE(CompileTypeF32, "f32", LLVMFloatType())
```

#### IrgenTest.CompileTypeIntArray
Tests integer array is compiled correctly
```c
TEST_TYPE(CompileTypeIntArray, "int[3]", LLVMArrayType(LLVMInt64Type(), 3))
```

#### IrgenTest.CompileTypeFloatArray
Tests float array is compiled correctly
```c
TEST_TYPE(CompileTypeFloatArray, "float[100]", LLVMArrayType(LLVMFloatType(), 100))
```

#### IrgenTest.CompileLiteralInt
Tests integer literal are compiled correctly
```c
TEST_LITERAL(CompileLiteralInt, "123", LLVMInt64Type(), "i64 123")
```

#### Irgentest.CompileLiteralFloat
Tests float literals are compiled correctly
```c
TEST_LITERAL(CompileLiteralFloat, "123.321", LLVMFloatType(), "float 0x405ED48B40000000")
```

#### IrgenTest.CompileLiteralHex
Tests hex literals are compiled correctly
```c
TEST_LITERAL(CompileLiteralHex, "0x1000", LLVMInt64Type(), "i64 4096")
```

#### IrgenTest.CompileLiteralOctal
Tests octal literals are compiled correctly
```c
TEST_LITERAL(CompileLiteralOctal, "0123", LLVMInt64Type(), "i64 83")
```

#### IrgenTest.I64ToI32
Tests cast from 64 bit integer to 32 bit integer
```c
TEST_CAST(I64ToI32, LLVMConstIntOfString(LLVMInt64Type(), "123", 10), LLVMInt32Type())
```

#### IrgenTest.I16ToFloat
Tests cast from 16 bit integer to float
```c
TEST_CAST(I16ToFloat, LLVMConstIntOfString(LLVMInt16Type(), "32", 10), LLVMFloatType())
```

#### IrgenTest.FloatToDouble
Tests cast from float to double
```c
TEST_CAST(FloatToDouble, LLVMConstRealOfString(LLVMFloatType(), "1.42"), LLVMDoubleType())
```

#### IrgenTest.DoubleToI64
Tests cast from double to 64 bit integer
```c
TEST_CAST(DoubleToI64, LLVMConstRealOfString(LLVMDoubleType(), "1245.12"), LLVMInt64Type())
```

### Integration tests
```c
LLVMGenericValueRef intArg(int n) {
    return LLVMCreateGenericValueOfInt(LLVMInt64Type(), n, false);
}

int runLLVMModule(Irgen *irgen) {

    // create an execution engine   
    LLVMExecutionEngineRef engine;
    char *error = (char *)NULL;

    # if 1
        // Initialize jit
        LLVMLinkInMCJIT();
    # else
        // Initialize intepreter
        LLVMLinkInInterpreter();
    # endif

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    assert(LLVMCreateExecutionEngineForModule(&engine, irgen->module, &error) == 0);
    assert(error == NULL);

    LLVMValueRef mainFunc; 
    LLVMFindFunction(engine, "main", &mainFunc);

    int res = LLVMRunFunctionAsMain(engine, mainFunc, 0, NULL, NULL);

    LLVMDisposeExecutionEngine(engine);
    return res;
}

void TEST_ERROR(char *src, TokenType type) {
    parser *p = new_parser(Lex(src));
    ast_unit *f = parse_file(p);
    
    ASSERT_EQ(queue_size(p->error_queue), 1);
    parser_error *err = (parser_error *)queue_pop_back(p->error_queue);
    ASSERT_EQ(err->expect_token.type, type);
    error(
        src, 
        err->start->line, 
        err->start->column, 
        err->start->column + err->length, 
        (char *)"Expected token %s", 
        TokenName(err->expect_token.type));
}

void TEST_MODULE(char *src, int out) {
    /* generate module */
    parser *p = new_parser(Lex(src));
    ast_unit *f = parse_file(p);
    
    Irgen *irgen = NewIrgen();
    
    for (int i = 0; i < f->dclCount; i++) {
        CompileFunction(irgen, f->dcls[i]);
    }

    // LLVMDumpModule(irgen->module);

    /* check for errors in module */
    char *error = (char *)NULL;
    LLVMVerifyModule(irgen->module, LLVMPrintMessageAction, &error);
    LLVMDisposeMessage(error);

    /* run the function */
    int res = runLLVMModule(irgen);
    ASSERT_EQ(out, res);

    /* dispose of builder */
    LLVMDisposeBuilder(irgen->builder);
}

char *loadTest(std::string name) {
    // build path to the file
    char *cname = (char *)name.c_str();
    char *pre = (char *)"../tests/tests/";
    int pathLen = strlen(pre) + strlen(cname) + 1;
    char *path = alloca(pathLen);
    sprintf(path, "%s%s", pre, cname);
    // printf("Path: %s\n", path);
    
    // open file
    FILE *f = fopen(path, "rb");
    assert(f != NULL);

    // get the length of the file
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    // read file into buffer
    char *buffer = (char *)malloc(length + 1);
    fread(buffer, 1, length, f);
    fclose(f);
    buffer[length] = '\0';
    
    return buffer;
}
```

#### IntegrationTest.CompileFunctionLiteral
Tests `literal.fur` compiles and returns the correct code

```c
proc main :: -> int {
    return 123
}
```

```c
TEST(IntegrationTest, CompileFunctionLiteral){ 
    TEST_MODULE(loadTest("literal.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/literal.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionBinaryInt
Tests `binaryInt.fur` compiles and returns the correct code

```c
proc main :: -> int {
    return 120 + 3
}
```

```c
TEST(IntegrationTest, CompileFunctionBinaryInt){ 
    TEST_MODULE(loadTest("binaryInt.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/binaryInt.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionBinaryFloat
Tests `binaryFloat.fur` compiles and returns the correct code

```c
proc main :: -> int {
    return 130.75 - 7.75
}
```

```c
TEST(IntegrationTest, CompileFunctionBinaryFloat){ 
    TEST_MODULE(loadTest("binaryFloat.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/binaryFloat.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionLongVar
Tests `longVar.fur` compiles and returns the correct code

```c
proc main :: -> int {
    var int a = 123
    return a
}
```

```c
TEST(IntegrationTest, CompileFunctionLongVar){ 
    TEST_MODULE(loadTest("longVar.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/longVar.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$123, -8(%rsp)
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionShortVar
Tests `shortVar.fur` compiles and returns the correct code

```c
proc main :: -> int {
    a := 123
    return a
}
```

```c
TEST(IntegrationTest, CompileFunctionShortVar){ 
    TEST_MODULE(loadTest("shortVar.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/shortVar.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$123, -8(%rsp)
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionIf
Tests `if.fur` compiles and returns the correct code

```c
proc main :: -> int {
    if true { 
        return 123 
    }
    return 321
}
```

```c
TEST(IntegrationTest, CompileFunctionIf){ 
    TEST_MODULE(loadTest("if.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/if.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %if
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionIfElse
Tests `ifElse.fur` compiles and returns the correct code

```c
proc main :: -> int {
    if true {
        return 123
    } else {
        return 321
    }
}
```

```c
TEST(IntegrationTest, CompileFunctionIfElse){ 
    TEST_MODULE(loadTest("ifElse.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/ifElse.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %if
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionIfElseIfElse
Tests `ifElseIfElse.fur` compiles and returns the correct code

```c
proc main :: -> int {
    if false {
        return 321
    } else if true {
        return 123
    } else {
        return 0
    }
}
```

```c
TEST(IntegrationTest, CompileFunctionIfElseIfElse){ 
    TEST_MODULE(loadTest("ifElseIfElse.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/ifElseIfElse.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %if1
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionIfElseIfelseIfElse
Tests `ifElseIfElseIfElse.fur` compiles and returns the correct code

```c
proc main :: -> int {
    if false {
        return 321
    } else if false {
        return 23
    } else if false {
        return 21
    } else {
        return 123
    }
}
```

```c
TEST(IntegrationTest, CompileFunctionIfElseIfElseIfElse){ 
    TEST_MODULE(loadTest("ifElseIfElseIfElse.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/ifElseIfElseIfElse.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %else4
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionFor
Tests `for.fur` compiles and returns the correct code

```c
proc main :: -> int {
    a := 0
    for i := 0; i < 123; i++ {
        a += 1
    }
    return a
}
```

```c
TEST(IntegrationTest, CompileFunctionFor){ 
    TEST_MODULE(loadTest("for.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/for.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$0, -8(%rsp)
	movq	$0, -16(%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_2
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Inner Loop Header: Depth=1
	incq	-8(%rsp)
	movq	-16(%rsp), %rax
	incq	%rax
	movq	%rax, -16(%rsp)
	cmpq	$123, %rax
	jl	.LBB0_1
.LBB0_2:                                # %endfor
	movq	-8(%rsp), %rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionArrayInit
Tests `arrayInit.fur` compiles and returns the correct code

```c
proc main :: -> int {
    a := [100, 20, 3]
    return a[0] + a[1] + a[2]
}
```

```c
TEST(IntegrationTest, CompileFunctionArrayInit){ 
    TEST_MODULE(loadTest("arrayInit.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/arrayInit.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$100, -24(%rsp)
	movq	$20, -16(%rsp)
	movq	$3, -8(%rsp)
	movq	-24(%rsp), %rax
	movq	-16(%rsp), %rcx
	leaq	3(%rax,%rcx), %rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionAdd
Tests `add.fur` compiles and returns the correct code

```c
proc add :: int a, int b -> int {
    return a + b
}

proc main :: -> int {
    return add(120, 3)
}
```

```c
TEST(IntegrationTest, CompileFunctionAdd){ 
    TEST_MODULE(loadTest("add.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/add.ll"
	.globl	add
	.align	16, 0x90
	.type	add,@function
add:                                    # @add
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movq	%rsi, -16(%rsp)
	addq	-8(%rsp), %rsi
	movq	%rsi, %rax
	retq
.Lfunc_end0:
	.size	add, .Lfunc_end0-add
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movl	$120, %edi
	movl	$3, %esi
	callq	add
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionUnary
Tests `unary.fur` compiles and returns the correct code

```c
proc neg :: int n -> int {
    return -n
}

proc main :: -> int {
    return neg(-123)
}
```

```c
TEST(IntegrationTest, CompileFunctionUnary){ 
    TEST_MODULE(loadTest("unary.fur"), 123); 
}
```

```assembly
	.text
	.file	"../tests/tests/unary.ll"
	.globl	neg
	.align	16, 0x90
	.type	neg,@function
neg:                                    # @neg
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	negq	%rdi
	movq	%rdi, %rax
	retq
.Lfunc_end0:
	.size	neg, .Lfunc_end0-neg
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movq	$-123, %rdi
	callq	neg
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionReassignArg
Tests `reassignArg.fur` compiles and returns the correct code

```c
proc test :: int n -> int {
    n = 123
    return n
}

proc main :: -> int {
    return test(321)
}
```

```c
TEST(IntegrationTest, CompileFunctionReassignArg){ 
    TEST_MODULE(loadTest("reassignArg.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/reassignArg.ll"
	.globl	test
	.align	16, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movq	$123, -8(%rsp)
	movl	$123, %eax
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movl	$321, %edi              # imm = 0x141
	callq	test
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionGCD
Tests `gcd.fur` compiles and returns the correct code

```c
proc gcd :: int a, int b -> int {
    if b == 0 {
        return a
    } 
    
    return gcd(b, a % b)
}

proc main :: -> int {
    return gcd(1529, 14039)
}
```

```c
TEST(IntegrationTest, CompileFunctionGCD){ 
    TEST_MODULE(loadTest("gcd.fur"), 139);
}
```

```assembly
	.text
	.file	"../tests/tests/gcd.ll"
	.globl	gcd
	.align	16, 0x90
	.type	gcd,@function
gcd:                                    # @gcd
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp0:
	.cfi_def_cfa_offset 32
	movq	%rdi, 16(%rsp)
	movq	%rsi, 8(%rsp)
	testq	%rsi, %rsi
	je	.LBB0_2
# BB#1:                                 # %endBlock
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rax
	cqto
	idivq	%rdi
	movq	%rdx, %rsi
	callq	gcd
	addq	$24, %rsp
	retq
.LBB0_2:                                # %if
	movq	16(%rsp), %rax
	addq	$24, %rsp
	retq
.Lfunc_end0:
	.size	gcd, .Lfunc_end0-gcd
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$1529, %edi             # imm = 0x5F9
	movl	$14039, %esi            # imm = 0x36D7
	callq	gcd
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionFibbonanci
Tests `fibbonanci.fur` compiles and returns the correct code

```c
proc fib :: int n -> int {
    if n < 2 {
        return n
    }

    return fib(n - 1) + fib(n - 2)
}

proc main :: -> int {
    return fib(12)
}
```

```c
TEST(IntegrationTest, CompileFunctionFibbonanci) {
    TEST_MODULE(loadTest("fibbonanci.fur"), 144);
}
```

```assembly
	.text
	.file	"../tests/tests/fibbonanci.ll"
	.globl	fib
	.align	16, 0x90
	.type	fib,@function
fib:                                    # @fib
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp0:
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
.Ltmp1:
	.cfi_def_cfa_offset 32
.Ltmp2:
	.cfi_offset %rbx, -16
	movq	%rdi, 8(%rsp)
	cmpq	$2, %rdi
	jge	.LBB0_1
# BB#2:                                 # %if
	movq	8(%rsp), %rax
	jmp	.LBB0_3
.LBB0_1:                                # %endBlock
	movq	8(%rsp), %rdi
	decq	%rdi
	callq	fib
	movq	%rax, %rbx
	movq	8(%rsp), %rdi
	addq	$-2, %rdi
	callq	fib
	addq	%rbx, %rax
.LBB0_3:                                # %if
	addq	$16, %rsp
	popq	%rbx
	retq
.Lfunc_end0:
	.size	fib, .Lfunc_end0-fib
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp3:
	.cfi_def_cfa_offset 16
	movl	$12, %edi
	callq	fib
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionArraySum
Tests `arraySum.fur` compiles and returns the correct code

```c
proc sum :: int[3] nums -> int {
    s := 0
    for i := 0; i < 3; i++ {
        s += nums[i]
    }

    return s
}

proc main :: -> int {
    a := [100, 20, 3]
    return sum(a)
}
```

```c
TEST(IntegrationTest, CompileFunctionArraySum) {
    TEST_MODULE(loadTest("arraySum.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/arraySum.ll"
	.globl	sum
	.align	16, 0x90
	.type	sum,@function
sum:                                    # @sum
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdx, -8(%rsp)
	movq	%rsi, -16(%rsp)
	movq	%rdi, -24(%rsp)
	movq	$0, -32(%rsp)
	movq	$0, -40(%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_2
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Inner Loop Header: Depth=1
	movq	-40(%rsp), %rax
	movq	-24(%rsp,%rax,8), %rax
	addq	%rax, -32(%rsp)
	movq	-40(%rsp), %rax
	incq	%rax
	movq	%rax, -40(%rsp)
	cmpq	$3, %rax
	jl	.LBB0_1
.LBB0_2:                                # %endfor
	movq	-32(%rsp), %rax
	retq
.Lfunc_end0:
	.size	sum, .Lfunc_end0-sum
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp0:
	.cfi_def_cfa_offset 32
	movq	$100, (%rsp)
	movq	$20, 8(%rsp)
	movq	$3, 16(%rsp)
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	movl	$3, %edx
	callq	sum
	addq	$24, %rsp
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionNestedFor
Tests `nestedFor.fur` compiles and returns the correct code

```c
proc main :: -> int {
    s := 0
    for i := 0; i < 3; i++ {
        for j := 0; j < 41; j++ {
            s += 1
        }
    }

    return s
}
```

```c
TEST(IntegrationTest, CompileFunctionNestedFor) {
    TEST_MODULE(loadTest("nestedFor.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/nestedFor.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	$0, -8(%rbp)
	movq	$0, -16(%rbp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_5
# BB#1:
	xorl	%eax, %eax
	.align	16, 0x90
.LBB0_2:                                # %for
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movq	%rsp, %rdx
	leaq	-16(%rdx), %rcx
	movq	%rcx, %rsp
	movq	$0, -16(%rdx)
	testb	%al, %al
	jne	.LBB0_4
	.align	16, 0x90
.LBB0_3:                                # %for1
                                        #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	incq	-8(%rbp)
	movq	(%rcx), %rdx
	incq	%rdx
	movq	%rdx, (%rcx)
	cmpq	$41, %rdx
	jl	.LBB0_3
.LBB0_4:                                # %endfor
                                        #   in Loop: Header=BB0_2 Depth=1
	movq	-16(%rbp), %rcx
	incq	%rcx
	movq	%rcx, -16(%rbp)
	cmpq	$3, %rcx
	jl	.LBB0_2
.LBB0_5:                                # %endfor7
	movq	-8(%rbp), %rax
	movq	%rbp, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionBubblesort
Tests `bubblesort.fur` compiles and returns the correct code

```c
proc sort :: int[5] items -> int[5] {
    n := 5
    for i := 0; i < n-1; i++ {
        for j := 0; j < n-i-1; j++ {
            if items[j] > items[j+1] {
                temp := items[j]
                items[j] = items[j+1]
                items[j+1] = temp
            }
        }
        n = 5
    }

    return items
}

proc main :: -> int {
    a := [54, 2, 42, 5, 6]
    a = sort(a)

    if a[0] < a[1] < a[2] < a[3] < a[4] < a[5] {
        return 123
    }
    
    return 0
}
```

```c
TEST(IntegrationTest, CompileFunctionBubblesort) {
    TEST_MODULE(loadTest("bubblesort.fur"), 123);
}
```

```assembly
	.text
	.file	"../tests/tests/bubblesort.ll"
	.globl	sort
	.align	16, 0x90
	.type	sort,@function
sort:                                   # @sort
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	%r9, -8(%rbp)
	movq	%r8, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	$5, -48(%rbp)
	movq	$0, -56(%rbp)
	movq	-48(%rbp), %rax
	decq	%rax
	testq	%rax, %rax
	jle	.LBB0_5
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_2 Depth 2
	movq	%rsp, %rcx
	leaq	-16(%rcx), %rax
	movq	%rax, %rsp
	movq	$0, -16(%rcx)
	movq	-48(%rbp), %rcx
	subq	-56(%rbp), %rcx
	decq	%rcx
	testq	%rcx, %rcx
	jle	.LBB0_4
	.align	16, 0x90
.LBB0_2:                                # %for1
                                        #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	(%rax), %rcx
	movq	-40(%rbp,%rcx,8), %rdx
	cmpq	-32(%rbp,%rcx,8), %rdx
	jle	.LBB0_3
# BB#6:                                 # %if
                                        #   in Loop: Header=BB0_2 Depth=2
	movq	(%rax), %rcx
	movq	-40(%rbp,%rcx,8), %rcx
	movq	%rsp, %rdx
	leaq	-16(%rdx), %rsp
	movq	%rcx, -16(%rdx)
	movq	(%rax), %rcx
	movq	-32(%rbp,%rcx,8), %rsi
	movq	%rsi, -40(%rbp,%rcx,8)
	movq	(%rax), %rcx
	movq	-16(%rdx), %rdx
	movq	%rdx, -32(%rbp,%rcx,8)
.LBB0_3:                                # %endBlock
                                        #   in Loop: Header=BB0_2 Depth=2
	movq	(%rax), %rcx
	incq	%rcx
	movq	%rcx, (%rax)
	movq	-48(%rbp), %rdx
	subq	-56(%rbp), %rdx
	decq	%rdx
	cmpq	%rdx, %rcx
	jl	.LBB0_2
.LBB0_4:                                # %endfor
                                        #   in Loop: Header=BB0_1 Depth=1
	movq	$5, -48(%rbp)
	movq	-56(%rbp), %rax
	incq	%rax
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rcx
	decq	%rcx
	cmpq	%rcx, %rax
	jl	.LBB0_1
.LBB0_5:                                # %endfor30
	movq	-40(%rbp), %r8
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	movq	%rax, 32(%rdi)
	movq	%rsi, 24(%rdi)
	movq	%rdx, 16(%rdi)
	movq	%rcx, 8(%rdi)
	movq	%r8, (%rdi)
	movq	%rdi, %rax
	movq	%rbp, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	sort, .Lfunc_end0-sort
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$88, %rsp
.Ltmp3:
	.cfi_def_cfa_offset 96
	movq	$54, 48(%rsp)
	movq	$2, 56(%rsp)
	movq	$42, 64(%rsp)
	movq	$5, 72(%rsp)
	movq	$6, 80(%rsp)
	movq	72(%rsp), %r8
	movq	64(%rsp), %rcx
	movq	48(%rsp), %rsi
	movq	56(%rsp), %rdx
	leaq	8(%rsp), %rdi
	movl	$6, %r9d
	callq	sort
	movq	8(%rsp), %rax
	movq	16(%rsp), %rcx
	movq	24(%rsp), %rdx
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdi
	movq	%rdi, 80(%rsp)
	movq	%rsi, 72(%rsp)
	movq	%rdx, 64(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%rax, 48(%rsp)
	movq	48(%rsp), %rax
	xorl	%ecx, %ecx
	cmpq	56(%rsp), %rax
	movq	$-1, %rax
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	64(%rsp), %rdx
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	72(%rsp), %rdx
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	80(%rsp), %rdx
	cmovlq	%rax, %rcx
	cmpq	88(%rsp), %rcx
	jge	.LBB1_1
# BB#2:                                 # %if
	movl	$123, %eax
	addq	$88, %rsp
	retq
.LBB1_1:                                # %endBlock
	xorl	%eax, %eax
	addq	$88, %rsp
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits

```

#### IntegrationTest.CompileFunctionProcColonError
Tests that functions without a double colon produce the correct error

```c
proc main -> int {
    return 123
}
```

```c
TEST(IntegrationTest, CompileFunctionProcColonError) {
    TEST_ERROR(loadTest("procColonError.fur"), DOUBLE_COLON);
}
```

![](https://i.imgur.com/x0Eli3B.png)

#### IntegrationTest.CompileFunctionProcArrowError
Tests that functions without an arrow prouduces the correct error

```c
proc main :: int a {
    return 123
}
```

```c
TEST(IntegrationTest, CompileFunctionProcArrowError) {
    TEST_ERROR(loadTest("procArrowError.fur"), ARROW);
}
```

![](https://i.imgur.com/9jX8bOH.png)

#### IntegrationTest.CompileFunctionProcNameError
Tests that functions without a name prouduces the correct error

```c
proc :: -> int {
    return 123
}
```

```c
TEST(IntegrationTest, CompileFunctionProcNameError) {
    TEST_ERROR(loadTest("procNameError.fur"), IDENT);
}
```

![](https://i.imgur.com/tyM40HK.png)

#### IntegrationTest.CompileFunctionVarEqualError
Tests that varible definititions without an equals produces the correct error

```c
proc main :: -> int {
    var int a 100
}
```

```c
TEST(IntegrationTest, CompileFunctionVarEqualError) {
    TEST_ERROR(loadTest("varEqualError.fur"), ASSIGN);
}
```

![](http://imgur.com/5CYpQya.png)

#### IntegrationTest.CompileFunctionVarNameError
Tests that a varible definition without a name produces the correct error

```c
proc main :: -> int {
    var int = 100
}
```

```c
TEST(IntegrationTest, CompileFunctionVarNameError) {
    TEST_ERROR(loadTest("varNameError.fur"), IDENT);
}
```

![](http://imgur.com/k4chiNf.png)
