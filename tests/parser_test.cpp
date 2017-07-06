#define TEST_EXPRESSION(source, expected) {                                 \
    auto exp = Parser((source)).parse_expression(0);                        \
    ASSERT_EQ((expected), *exp);                                            \
}                                                                           \

#define TEST_STATEMENT(source, expected) {                                  \
    auto exp = Parser((source)).parse_statement();                          \
    ASSERT_EQ((expected), *exp);                                            \
}                                                                           \

#define TEST_FUNCTION(source, expected) {                                   \
    auto func = Parser((source)).parse_function();                          \
    ASSERT_EQ((expected), *func);                                           \
}                                                                           \

TEST(ParserTest, EmptyFunction) {
    TEST_FUNCTION("proc foo :: -> {}", Function(
        "foo",
        {},
        {},
        new BlockStatement({})
    ))

    TEST_FUNCTION("proc foo :: int a, int b -> {}", Function(
        "foo",
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "a"),
            std::make_tuple(new PrimitiveType(Primitive::INT), "b"),
        },
        {},
        new BlockStatement({})
    ))

    TEST_FUNCTION("proc foo :: int a, int b -> float c, float d {}", Function(
        "foo",
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "a"),
            std::make_tuple(new PrimitiveType(Primitive::INT), "b"),
        },
        {
            std::make_tuple(new PrimitiveType(Primitive::FLOAT), "c"),
            std::make_tuple(new PrimitiveType(Primitive::FLOAT), "d"),
        },
        new BlockStatement({})
    ))

    TEST_FUNCTION("proc foo :: int a -> int b { return a + 1; }", Function(
        "foo",
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "a"),
        },
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "b"),
        },
        new BlockStatement({
            new ReturnStatement(
                new BinaryExpression(
                    TokenType::ADD,
                    new IdentExpression("a"),
                    new LiteralExpression(TokenType::INT, "1")
                )
            )
        })
    ))
}

TEST(ParserTest, ParseReturnStatement) {
    TEST_STATEMENT("return a;", ReturnStatement(new IdentExpression("a")))
}

TEST(ParserTest, ParseLiteral) {
    TEST_EXPRESSION("100", LiteralExpression(TokenType::INT, "100"))
    TEST_EXPRESSION("10.01", LiteralExpression(TokenType::FLOAT, "10.01"))
    TEST_EXPRESSION("0240", LiteralExpression(TokenType::OCTAL, "240"))
    TEST_EXPRESSION("0x1000", LiteralExpression(TokenType::HEX, "1000"))
}

TEST(ParserTest, ParseUnaryExpression) {
    TEST_EXPRESSION("!foo", UnaryExpression(TokenType::NOT, new IdentExpression("foo")))
    TEST_EXPRESSION("-foo", UnaryExpression(TokenType::SUB, new IdentExpression("foo")))
}

TEST(ParserTest, ParseBinaryExpression) {
    TEST_EXPRESSION("foo + bar", BinaryExpression(
        TokenType::ADD, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))
    
    TEST_EXPRESSION("foo - bar", BinaryExpression(
        TokenType::SUB, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo * bar", BinaryExpression(
        TokenType::MUL, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo / bar", BinaryExpression(
        TokenType::QUO, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo % bar", BinaryExpression(
        TokenType::REM, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo == bar", BinaryExpression(
        TokenType::EQL, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo != bar", BinaryExpression(
        TokenType::NEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo > bar", BinaryExpression(
        TokenType::GTR, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo < bar", BinaryExpression(
        TokenType::LSS, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo >= bar", BinaryExpression(
        TokenType::GEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo <= bar", BinaryExpression(
        TokenType::LEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))
}

TEST(ParserTest, ParseCallExpression) {
    TEST_EXPRESSION("a()", 
        CallExpression(
            new IdentExpression("a"), {})
    )

    TEST_EXPRESSION("a(b)", 
        CallExpression(
            new IdentExpression("a"), {
                new IdentExpression("b")
            })
    )

    TEST_EXPRESSION("a(b, c)", 
        CallExpression(
            new IdentExpression("a"), {
                new IdentExpression("b"),
                new IdentExpression("c"),
            })
    )
    
    TEST_EXPRESSION("a(1 + 2, a - b)", 
        CallExpression(
            new IdentExpression("a"), {
                new BinaryExpression(
                    TokenType::ADD,
                    new LiteralExpression(TokenType::INT, "1"),
                    new LiteralExpression(TokenType::INT, "2")
                ),
                new BinaryExpression(
                    TokenType::SUB,
                    new IdentExpression("a"),
                    new IdentExpression("b")
                )
            })
    )
}

TEST(ParserTest, ParseBlockStatements) {
        TEST_STATEMENT("{}", BlockStatement({}))

        TEST_STATEMENT("{ return a; }", BlockStatement({
            new ReturnStatement(
                new IdentExpression("a")
            ),
        }))
        
        TEST_STATEMENT("{ return a; return b; }", BlockStatement({
            new ReturnStatement(
                new IdentExpression("a")
            ),
            new ReturnStatement(
                new IdentExpression("b")
            ),
        }))

        TEST_STATEMENT("{{{}}}", BlockStatement({
            new BlockStatement({
                new BlockStatement({}),
            }),
        }))
}

TEST(ParserTest, ParseIfStatement) {
    TEST_STATEMENT("if foo {}", IfStatement(
        new IdentExpression("foo"),
        NULL,
        new BlockStatement({})
    ))

    TEST_STATEMENT("if foo {} else {}", IfStatement(
        new IdentExpression("foo"),
        new IfStatement(
            NULL,
            NULL,
            new BlockStatement({})
        ),
        new BlockStatement({})
    ))

    TEST_STATEMENT("if foo {} else if bar {} else {}", IfStatement(
        new IdentExpression("foo"),
        new IfStatement(
            new IdentExpression("bar"),
            new IfStatement(
                NULL,
                NULL,
                new BlockStatement({})
            ),
            new BlockStatement({})
        ),
        new BlockStatement({})
    ))
}

TEST(ParserTest, ParseForStatement) {
    TEST_STATEMENT("for return a; a < 20; return a {}", ForStatement(
        new ReturnStatement(
            new IdentExpression("a")
        ),
        new BinaryExpression(
            TokenType::LSS,
            new IdentExpression("a"),
            new LiteralExpression(TokenType::INT, "20")
        ),
        new ReturnStatement(
            new IdentExpression("a")
        ),
        new BlockStatement({})
    ))
}

TEST(ParserTest, ParseAssignStatements) {
    TEST_STATEMENT("foo := 100", AssignStatement(
        new IdentExpression("foo"),
        TokenType::DEFINE,
        new LiteralExpression(TokenType::INT, "100")
    ))

    TEST_STATEMENT("foo = bar", AssignStatement(
        new IdentExpression("foo"),
        TokenType::ASSIGN,
        new IdentExpression("bar")
    ))

    TEST_STATEMENT("baz += 100 + 20", AssignStatement(
        new IdentExpression("baz"),
        TokenType::ADD_ASSIGN,
        new BinaryExpression(
            TokenType::ADD,
            new LiteralExpression(TokenType::INT, "100"),
            new LiteralExpression(TokenType::INT, "20")
        )
    ))
}