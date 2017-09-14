TEST_SUITE_BEGIN("Parser");

#define TEST_EXPRESSION(source, expected) {                                 \
    SUBCASE(#source) {                                                      \
        auto exp = Parser((source)).parse_expression(0);                    \
        REQUIRE_EQ((expected), *exp);                                         \
    }                                                                       \
}                                                                           \

#define TEST_STATEMENT(source, expected) {                                  \
    SUBCASE(#source) {                                                      \
        auto smt = Parser((source)).parse_statement();                      \
        REQUIRE_EQ((expected), *smt);                                         \
    }                                                                       \
}                                                                           \

#define TEST_FUNCTION(source, expected) {                                   \
    SUBCASE(#source) {                                                      \
        auto func = Parser((source)).parse_function();                      \
        REQUIRE_EQ((expected), *func);                                        \
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
                new BinaryExpression(
                    TokenType::ADD,
                    new IdentExpression("a"),
                    new LiteralExpression(TokenType::INT, "1")
                )
            )
        })
    ))
}

TEST_CASE("return statement") {
    TEST_STATEMENT("return a;", *Statement::Return(new IdentExpression("a")))
}

TEST_CASE("literal expression") {
    TEST_EXPRESSION("100", *new LiteralExpression(TokenType::INT, "100"))
    TEST_EXPRESSION("10.01", *new LiteralExpression(TokenType::FLOAT, "10.01"))
    TEST_EXPRESSION("0240", *new LiteralExpression(TokenType::OCTAL, "240"))
    TEST_EXPRESSION("0x1000", *new LiteralExpression(TokenType::HEX, "1000"))
}

TEST_CASE("unary expression") {
    TEST_EXPRESSION("!foo", *new UnaryExpression(TokenType::NOT, new IdentExpression("foo")))
    TEST_EXPRESSION("-foo", *new UnaryExpression(TokenType::SUB, new IdentExpression("foo")))
}

TEST_CASE("binary expression") {
    TEST_EXPRESSION("foo + bar", *new BinaryExpression(
        TokenType::ADD, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))
    
    TEST_EXPRESSION("foo - bar", *new BinaryExpression(
        TokenType::SUB, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo * bar", *new BinaryExpression(
        TokenType::MUL, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo / bar", *new BinaryExpression(
        TokenType::QUO, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo % bar", *new BinaryExpression(
        TokenType::REM, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo == bar", *new BinaryExpression(
        TokenType::EQL, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo != bar", *new BinaryExpression(
        TokenType::NEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo > bar", *new BinaryExpression(
        TokenType::GTR, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo < bar", *new BinaryExpression(
        TokenType::LSS, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo >= bar", *new BinaryExpression(
        TokenType::GEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo <= bar", *new BinaryExpression(
        TokenType::LEQ, 
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))
}

TEST_CASE("call expression") {
    TEST_EXPRESSION("a()", 
        *new CallExpression(
            "a", {})
    )

    TEST_EXPRESSION("a(b)", 
        *new CallExpression(
            "a", {
                new IdentExpression("b")
            })
    )

    TEST_EXPRESSION("a(b, c)", 
        *new CallExpression(
            "a", {
                new IdentExpression("b"),
                new IdentExpression("c"),
            })
    )
    
    TEST_EXPRESSION("a(1 + 2, a - b)", 
        *new CallExpression(
            "a", {
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

TEST_CASE("block statement") {
        TEST_STATEMENT("{}", *Statement::Block({}))

        TEST_STATEMENT("{ return a; }", *Statement::Block({
            Statement::Return(
                new IdentExpression("a")
            ),
        }))
        
        TEST_STATEMENT("{ return a; return b; }", *Statement::Block({
            Statement::Return(
                new IdentExpression("a")
            ),
            Statement::Return(
                new IdentExpression("b")
            ),
        }))

        TEST_STATEMENT("{ a := 10; b := -a; }", *Statement::Block({
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::DEFINE,
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("b"),
                TokenType::DEFINE,
                new UnaryExpression(
                    TokenType::SUB,
                    new IdentExpression("a")
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
        new IdentExpression("foo"),
        Statement::Block({}),
        NULL
    ))

    TEST_STATEMENT("if foo {} else {}", *Statement::If(
        new IdentExpression("foo"),
        Statement::Block({}),
        Statement::If(
            NULL,
            Statement::Block({}),
            NULL
        )
    ))

    TEST_STATEMENT("if foo {} else if bar {} else {}", *Statement::If(
        new IdentExpression("foo"),
        Statement::Block({}),
        Statement::If(
            new IdentExpression("bar"),
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
            new IdentExpression("a"),
            TokenType::DEFINE,
            new LiteralExpression(TokenType::INT, "0")
        ),
        new BinaryExpression(
            TokenType::LSS,
            new IdentExpression("a"),
            new LiteralExpression(TokenType::INT, "20")
        ),
        Statement::Assign(
            new IdentExpression("a"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "1")
        ),
        Statement::Block({})
    ))
}

TEST_CASE("assign statement") {
    TEST_STATEMENT("foo := 100", *Statement::Assign(
        new IdentExpression("foo"),
        TokenType::DEFINE,
        new LiteralExpression(TokenType::INT, "100")
    ))

    TEST_STATEMENT("foo = bar", *Statement::Assign(
        new IdentExpression("foo"),
        TokenType::ASSIGN,
        new IdentExpression("bar")
    ))

    TEST_STATEMENT("baz += 100 + 20", *Statement::Assign(
        new IdentExpression("baz"),
        TokenType::ADD_ASSIGN,
        new BinaryExpression(
            TokenType::ADD,
            new LiteralExpression(TokenType::INT, "100"),
            new LiteralExpression(TokenType::INT, "20")
        )
    ))
}

TEST_CASE("multiple proc statements") {
    auto funcs = Parser("proc main :: -> {}\n\n"
                        "proc bar :: -> {}").parse();
    REQUIRE_EQ(funcs.size(), 2);

    REQUIRE_EQ(*funcs[0], Function("main", {}, {}, Statement::Block({})));
    REQUIRE_EQ(*funcs[1], Function("bar", {}, {}, Statement::Block({})));
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
        MESSAGE("Parsing: " << acl_string_path);
        auto funcs = Parser(acl).parse();
	}
}    

TEST_SUITE_END();
