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
                    TokenType::SYMBOL_ADD,
                    new IdentExpression("a"),
                    new LiteralExpression(TokenType::LITERAL_INT, "1")
                )
            )
        })
    ))
}

TEST_CASE("return statement") {
    TEST_STATEMENT("return a;", *new ReturnStatement(new IdentExpression("a")))
}

TEST_CASE("literal expression") {
    TEST_EXPRESSION("100", *new LiteralExpression(TokenType::LITERAL_INT, "100"))
    TEST_EXPRESSION("10.01", *new LiteralExpression(TokenType::LITERAL_FLOAT, "10.01"))
    TEST_EXPRESSION("0240", *new LiteralExpression(TokenType::LITERAL_OCTAL, "240"))
    TEST_EXPRESSION("0x1000", *new LiteralExpression(TokenType::LITERAL_HEX, "1000"))
}

TEST_CASE("unary expression") {
    TEST_EXPRESSION("!foo", *new UnaryExpression(TokenType::SYMBOL_NOT, new IdentExpression("foo")))
    TEST_EXPRESSION("-foo", *new UnaryExpression(TokenType::SYMBOL_SUB, new IdentExpression("foo")))
}

TEST_CASE("binary expression") {
    TEST_EXPRESSION("foo + bar", *new BinaryExpression(
        TokenType::SYMBOL_ADD,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))
    
    TEST_EXPRESSION("foo - bar", *new BinaryExpression(
        TokenType::SYMBOL_SUB,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo * bar", *new BinaryExpression(
        TokenType::SYMBOL_MUL,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo / bar", *new BinaryExpression(
        TokenType::SYMBOL_QUO,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo % bar", *new BinaryExpression(
        TokenType::SYMBOL_REM,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo == bar", *new BinaryExpression(
        TokenType::SYMBOL_EQL,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo != bar", *new BinaryExpression(
        TokenType::SYMBOL_NEQ,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo > bar", *new BinaryExpression(
        TokenType::SYMBOL_GTR,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo < bar", *new BinaryExpression(
        TokenType::SYMBOL_LSS,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo >= bar", *new BinaryExpression(
        TokenType::SYMBOL_GEQ,
        new IdentExpression("foo"), 
        new IdentExpression("bar")
    ))

    TEST_EXPRESSION("foo <= bar", *new BinaryExpression(
        TokenType::SYMBOL_LEQ,
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
                    TokenType::SYMBOL_ADD,
                    new LiteralExpression(TokenType::LITERAL_INT, "1"),
                    new LiteralExpression(TokenType::LITERAL_INT, "2")
                ),
                new BinaryExpression(
                    TokenType::SYMBOL_SUB,
                    new IdentExpression("a"),
                    new IdentExpression("b")
                )
            })
    )
}

TEST_CASE("block statement") {
        TEST_STATEMENT("{}", *new BlockStatement({}))

        TEST_STATEMENT("{ return a; }", *new BlockStatement({
            new ReturnStatement(
                new IdentExpression("a")
            ),
        }))
        
        TEST_STATEMENT("{ return a; return b; }", *new BlockStatement({
            new ReturnStatement(
                new IdentExpression("a")
            ),
            new ReturnStatement(
                new IdentExpression("b")
            ),
        }))

        TEST_STATEMENT("{ a := 10; b := -a; }", *new BlockStatement({
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::SYMBOL_DEFINE,
                new LiteralExpression(TokenType::LITERAL_INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("b"),
                TokenType::SYMBOL_DEFINE,
                new UnaryExpression(
                    TokenType::SYMBOL_SUB,
                    new IdentExpression("a")
                )
            )
        }))

        TEST_STATEMENT("{{{}}}", *new BlockStatement({
            new BlockStatement({
                new BlockStatement({}),
            }),
        }))
}

TEST_CASE("if statement") {
    TEST_STATEMENT("if foo {}", *new IfStatement(
        new IdentExpression("foo"),
        new BlockStatement({}),
        NULL
    ))

    TEST_STATEMENT("if foo {} else {}", *new IfStatement(
        new IdentExpression("foo"),
        new BlockStatement({}),
        new IfStatement(
            NULL,
            new BlockStatement({}),
            NULL
        )
    ))

    TEST_STATEMENT("if foo {} else if bar {} else {}", *new IfStatement(
        new IdentExpression("foo"),
        new BlockStatement({}),
        new IfStatement(
            new IdentExpression("bar"),
            new BlockStatement({}),
            new IfStatement(
                NULL,
                new BlockStatement({}),
                NULL
            )
        )
    ))
}

TEST_CASE("for statement") {
    TEST_STATEMENT("for a := 0; a < 20; a += 1 {}", *new ForStatement(
        new AssignStatement(
            new IdentExpression("a"),
            TokenType::SYMBOL_DEFINE,
            new LiteralExpression(TokenType::LITERAL_INT, "0")
        ),
        new BinaryExpression(
            TokenType::SYMBOL_LSS,
            new IdentExpression("a"),
            new LiteralExpression(TokenType::LITERAL_INT, "20")
        ),
        new AssignStatement(
            new IdentExpression("a"),
            TokenType::SYMBOL_ADD_ASSIGN,
            new LiteralExpression(TokenType::LITERAL_INT, "1")
        ),
        new BlockStatement({})
    ))
}

TEST_CASE("assign statement") {
    TEST_STATEMENT("foo := 100", *new AssignStatement(
        new IdentExpression("foo"),
        TokenType::SYMBOL_DEFINE,
        new LiteralExpression(TokenType::LITERAL_INT, "100")
    ))

    TEST_STATEMENT("foo = bar", *new AssignStatement(
        new IdentExpression("foo"),
        TokenType::SYMBOL_ASSIGN,
        new IdentExpression("bar")
    ))

    TEST_STATEMENT("baz += 100 + 20", *new AssignStatement(
        new IdentExpression("baz"),
        TokenType::SYMBOL_ADD_ASSIGN,
        new BinaryExpression(
            TokenType::SYMBOL_ADD,
            new LiteralExpression(TokenType::LITERAL_INT, "100"),
            new LiteralExpression(TokenType::LITERAL_INT, "20")
        )
    ))
}

TEST_CASE("multiple proc statements") {
    auto funcs = Parser("proc main :: -> {}\n\n"
                        "proc bar :: -> {}").parse();
    REQUIRE_EQ(funcs.size(), 2);

    REQUIRE_EQ(*funcs[0], Function("main", {}, {}, new BlockStatement({})));
    REQUIRE_EQ(*funcs[1], Function("bar", {}, {}, new BlockStatement({})));
}

TEST_CASE("acl test files") {
#ifdef _WIN32
    std::string acl_path = get_working_path() + "\\..\\tests\\acl\\";
	std::string acl_find_path = acl_path + "*.acl";

    HANDLE h_find;
    WIN32_FIND_DATA data;
    h_find = FindFirstFileA(acl_find_path.c_str(), &data);

    if(h_find != INVALID_HANDLE_VALUE) {
        do {
            std::string path = acl_path + data.cFileName;
            std::ifstream acl_file(path);
            std::string acl((std::istreambuf_iterator<char>(acl_file)),
                std::istreambuf_iterator<char>());

            // check it parses
            MESSAGE("Parsing: " << path);
            auto funcs = Parser(acl).parse();
        } while(FindNextFile(h_find, &data));

        FindClose(h_find);
    }
#else
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
#endif
}    

TEST_SUITE_END();
