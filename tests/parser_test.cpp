TEST_SUITE_BEGIN("Parser");

#define TEST_EXPRESSION(source, expected) {                                 \
    SUBCASE(#source) {                                                      \
        auto exp = Parser((source)).parse_expression(0);                    \
        CHECK_EQ((expected), *exp);                                         \
    }                                                                       \
}                                                                           \

#define TEST_STATEMENT(source, expected) {                                  \
    SUBCASE(#source) {                                                      \
        auto smt = Parser((source)).parse_statement();                      \
        CHECK_EQ((expected), *smt);                                         \
    }                                                                       \
}                                                                           \

#define TEST_FUNCTION(source, expected) {                                   \
    SUBCASE(#source) {                                                      \
        auto func = Parser((source)).parse_function();                      \
        CHECK_EQ((expected), *func);                                        \
    }                                                                       \
}                                                                           \

TEST_CASE("functions") {
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
                Expression::Binary(
                    TokenType::ADD,
                    Expression::Ident("a"),
                    Expression::Literal(TokenType::INT, "1")
                )
            )
        })
    ))
}

TEST_CASE("return statement") {
    TEST_STATEMENT("return a;", ReturnStatement(Expression::Ident("a")))
}

TEST_CASE("literal expression") {
    TEST_EXPRESSION("100", *Expression::Literal(TokenType::INT, "100"))
    TEST_EXPRESSION("10.01", *Expression::Literal(TokenType::FLOAT, "10.01"))
    TEST_EXPRESSION("0240", *Expression::Literal(TokenType::OCTAL, "240"))
    TEST_EXPRESSION("0x1000", *Expression::Literal(TokenType::HEX, "1000"))
}

TEST_CASE("unary expression") {
    TEST_EXPRESSION("!foo", *Expression::Unary(TokenType::NOT, Expression::Ident("foo")))
    TEST_EXPRESSION("-foo", *Expression::Unary(TokenType::SUB, Expression::Ident("foo")))
}

TEST_CASE("binary expression") {
    TEST_EXPRESSION("foo + bar", *Expression::Binary(
        TokenType::ADD, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))
    
    TEST_EXPRESSION("foo - bar", *Expression::Binary(
        TokenType::SUB, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo * bar", *Expression::Binary(
        TokenType::MUL, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo / bar", *Expression::Binary(
        TokenType::QUO, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo % bar", *Expression::Binary(
        TokenType::REM, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo == bar", *Expression::Binary(
        TokenType::EQL, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo != bar", *Expression::Binary(
        TokenType::NEQ, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo > bar", *Expression::Binary(
        TokenType::GTR, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo < bar", *Expression::Binary(
        TokenType::LSS, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo >= bar", *Expression::Binary(
        TokenType::GEQ, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))

    TEST_EXPRESSION("foo <= bar", *Expression::Binary(
        TokenType::LEQ, 
        Expression::Ident("foo"), 
        Expression::Ident("bar")
    ))
}

TEST_CASE("call expression") {
    TEST_EXPRESSION("a()", 
        *Expression::Call(
            "a", {})
    )

    TEST_EXPRESSION("a(b)", 
        *Expression::Call(
            "a", {
                Expression::Ident("b")
            })
    )

    TEST_EXPRESSION("a(b, c)", 
        *Expression::Call(
            "a", {
                Expression::Ident("b"),
                Expression::Ident("c"),
            })
    )
    
    TEST_EXPRESSION("a(1 + 2, a - b)", 
        *Expression::Call(
            "a", {
                Expression::Binary(
                    TokenType::ADD,
                    Expression::Literal(TokenType::INT, "1"),
                    Expression::Literal(TokenType::INT, "2")
                ),
                Expression::Binary(
                    TokenType::SUB,
                    Expression::Ident("a"),
                    Expression::Ident("b")
                )
            })
    )
}

TEST_CASE("block statement") {
        TEST_STATEMENT("{}", BlockStatement({}))

        TEST_STATEMENT("{ return a; }", BlockStatement({
            new ReturnStatement(
                Expression::Ident("a")
            ),
        }))
        
        TEST_STATEMENT("{ return a; return b; }", BlockStatement({
            new ReturnStatement(
                Expression::Ident("a")
            ),
            new ReturnStatement(
                Expression::Ident("b")
            ),
        }))

        TEST_STATEMENT("{{{}}}", BlockStatement({
            new BlockStatement({
                new BlockStatement({}),
            }),
        }))
}

TEST_CASE("if statement") {
    TEST_STATEMENT("if foo {}", IfStatement(
        Expression::Ident("foo"),
        NULL,
        new BlockStatement({})
    ))

    TEST_STATEMENT("if foo {} else {}", IfStatement(
        Expression::Ident("foo"),
        new IfStatement(
            NULL,
            NULL,
            new BlockStatement({})
        ),
        new BlockStatement({})
    ))

    TEST_STATEMENT("if foo {} else if bar {} else {}", IfStatement(
        Expression::Ident("foo"),
        new IfStatement(
            Expression::Ident("bar"),
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

TEST_CASE("for statement") {
    TEST_STATEMENT("for a := 0; a < 20; a += 1 {}", ForStatement(
        new AssignStatement(
            Expression::Ident("a"),
            TokenType::DEFINE,
            Expression::Literal(TokenType::INT, "0")
        ),
        Expression::Binary(
            TokenType::LSS,
            Expression::Ident("a"),
            Expression::Literal(TokenType::INT, "20")
        ),
        new AssignStatement(
            Expression::Ident("a"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "1")
        ),
        new BlockStatement({})
    ))
}

TEST_CASE("assign statement") {
    TEST_STATEMENT("foo := 100", AssignStatement(
        Expression::Ident("foo"),
        TokenType::DEFINE,
        Expression::Literal(TokenType::INT, "100")
    ))

    TEST_STATEMENT("foo = bar", AssignStatement(
        Expression::Ident("foo"),
        TokenType::ASSIGN,
        Expression::Ident("bar")
    ))

    TEST_STATEMENT("baz += 100 + 20", AssignStatement(
        Expression::Ident("baz"),
        TokenType::ADD_ASSIGN,
        Expression::Binary(
            TokenType::ADD,
            Expression::Literal(TokenType::INT, "100"),
            Expression::Literal(TokenType::INT, "20")
        )
    ))
}

TEST_SUITE_END();
