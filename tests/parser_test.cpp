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
        Statement::Block({})
    ))

    TEST_FUNCTION("proc foo :: int a, int b -> {}", Function(
        "foo",
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "a"),
            std::make_tuple(new PrimitiveType(Primitive::INT), "b"),
        },
        {},
        Statement::Block({})
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
        Statement::Block({})
    ))

    TEST_FUNCTION("proc foo :: int a -> int b { return a + 1; }", Function(
        "foo",
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "a"),
        },
        {
            std::make_tuple(new PrimitiveType(Primitive::INT), "b"),
        },
        Statement::Block({
            Statement::Return(
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
    TEST_STATEMENT("return a;", *Statement::Return(Expression::Ident("a")))
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
        TEST_STATEMENT("{}", *Statement::Block({}))

        TEST_STATEMENT("{ return a; }", *Statement::Block({
            Statement::Return(
                Expression::Ident("a")
            ),
        }))
        
        TEST_STATEMENT("{ return a; return b; }", *Statement::Block({
            Statement::Return(
                Expression::Ident("a")
            ),
            Statement::Return(
                Expression::Ident("b")
            ),
        }))

        TEST_STATEMENT("{ a := 10; b := -a; }", *Statement::Block({
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::DEFINE,
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("b"),
                TokenType::DEFINE,
                Expression::Unary(
                    TokenType::SUB,
                    Expression::Ident("a")
                )
            )
        }))

        TEST_STATEMENT("{{{}}}", *Statement::Block({
            Statement::Block({
                Statement::Block({}),
            }),
        }))
}

TEST_CASE("if statement") {
    TEST_STATEMENT("if foo {}", *Statement::If(
        Expression::Ident("foo"),
        Statement::Block({}),
        NULL
    ))

    TEST_STATEMENT("if foo {} else {}", *Statement::If(
        Expression::Ident("foo"),
        Statement::Block({}),
        Statement::If(
            NULL,
            Statement::Block({}),
            NULL
        )
    ))

    TEST_STATEMENT("if foo {} else if bar {} else {}", *Statement::If(
        Expression::Ident("foo"),
        Statement::Block({}),
        Statement::If(
            Expression::Ident("bar"),
            Statement::Block({}),
            Statement::If(
                NULL,
                Statement::Block({}),
                NULL
            )
        )
    ))
}

TEST_CASE("for statement") {
    TEST_STATEMENT("for a := 0; a < 20; a += 1 {}", *Statement::For(
        Statement::Assign(
            Expression::Ident("a"),
            TokenType::DEFINE,
            Expression::Literal(TokenType::INT, "0")
        ),
        Expression::Binary(
            TokenType::LSS,
            Expression::Ident("a"),
            Expression::Literal(TokenType::INT, "20")
        ),
        Statement::Assign(
            Expression::Ident("a"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "1")
        ),
        Statement::Block({})
    ))
}

TEST_CASE("assign statement") {
    TEST_STATEMENT("foo := 100", *Statement::Assign(
        Expression::Ident("foo"),
        TokenType::DEFINE,
        Expression::Literal(TokenType::INT, "100")
    ))

    TEST_STATEMENT("foo = bar", *Statement::Assign(
        Expression::Ident("foo"),
        TokenType::ASSIGN,
        Expression::Ident("bar")
    ))

    TEST_STATEMENT("baz += 100 + 20", *Statement::Assign(
        Expression::Ident("baz"),
        TokenType::ADD_ASSIGN,
        Expression::Binary(
            TokenType::ADD,
            Expression::Literal(TokenType::INT, "100"),
            Expression::Literal(TokenType::INT, "20")
        )
    ))
}

TEST_CASE("multiple proc statements") {
    auto funcs = Parser("proc main :: -> {}\n\n"
                        "proc bar :: -> {}").parse();
    CHECK_EQ(funcs.size(), 2);

    CHECK_EQ(*funcs[0], Function("main", {}, {}, Statement::Block({})));
    CHECK_EQ(*funcs[1], Function("bar", {}, {}, Statement::Block({})));
}

TEST_CASE("acl test files") {
    namespace fs = std::experimental::filesystem;
	std::string acls_path = "../tests/acl/";
	for (auto &p : fs::directory_iterator(acls_path)) {
		// read file
		std::string acl_string_path = p.path().string();
		fs::path acl_path = fs::path(acl_string_path);
		std::ifstream acl_file(acl_path.string());
		std::string acl((std::istreambuf_iterator<char>(acl_file)),
			std::istreambuf_iterator<char>());
        
        // check it parses
        CHECK_EQ(acl_string_path, acl_string_path);
        auto funcs = Parser(acl).parse();
	}
}    

TEST_SUITE_END();
