TEST_SUITE_BEGIN("Ast");

TEST_CASE("identifier expressions equal") {
    CHECK_EQ(IdentExpression("foo"), IdentExpression("foo"));
    CHECK_NE(IdentExpression("foo"), IdentExpression("bar"));
}

TEST_CASE("literal expressions equal") {
    CHECK_EQ(LiteralExpression(TokenType::INT, "100"), LiteralExpression(TokenType::INT, "100"));
    CHECK_NE(LiteralExpression(TokenType::INT, "100"), LiteralExpression(TokenType::FLOAT, "100"));
    CHECK_NE(LiteralExpression(TokenType::INT, "100"), LiteralExpression(TokenType::INT, "200"));
}

TEST_CASE("unary expressions equal") {
    CHECK_EQ(
        UnaryExpression(TokenType::NOT, new IdentExpression("a")), 
        UnaryExpression(TokenType::NOT, new IdentExpression("a"))
    );
    
    CHECK_NE(
        UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        UnaryExpression(TokenType::NOT, new IdentExpression("b"))
    );

    CHECK_NE(
        UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        UnaryExpression(TokenType::SUB, new IdentExpression("a"))
    );

    CHECK_NE(
        UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        UnaryExpression(TokenType::NOT, new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("binary expressions equal") {
    CHECK_EQ(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b"))
    );

    CHECK_NE(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::SUB, new IdentExpression("a"), new IdentExpression("b"))
    );

    CHECK_NE(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::ADD, new IdentExpression("b"), new IdentExpression("b"))
    );

    CHECK_NE(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("a"))
    );

    CHECK_NE(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::ADD, new LiteralExpression(TokenType::INT, "100"), new IdentExpression("b"))
    );

    CHECK_NE(
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        BinaryExpression(TokenType::ADD, new IdentExpression("a"), new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("call expressions equal") {
    CHECK_EQ(
        CallExpression(new IdentExpression("foo"), {}),
        CallExpression(new IdentExpression("foo"), {})
    );

    CHECK_NE(
        CallExpression(new IdentExpression("foo"), {}),
        CallExpression(new IdentExpression("bar"), {})
    );

    CHECK_EQ(
        CallExpression(new IdentExpression("foo"), {new IdentExpression("a")}),
        CallExpression(new IdentExpression("foo"), {new IdentExpression("a")})
    );

    CHECK_NE(
        CallExpression(new IdentExpression("foo"), {new IdentExpression("a")}),
        CallExpression(new IdentExpression("foo"), {})
    );

    CHECK_NE(
        CallExpression(new IdentExpression("foo"), {new IdentExpression("a")}),
        CallExpression(new IdentExpression("foo"), {new IdentExpression("b")})
    );

    CHECK_NE(
        CallExpression(new IdentExpression("foo"), {new IdentExpression("a")}),
        CallExpression(new IdentExpression("foo"), {new LiteralExpression(TokenType::INT, "100")})
    );
}

TEST_CASE("return statements equal") {
    CHECK_EQ(
        ReturnStatement(new IdentExpression("a")),
        ReturnStatement(new IdentExpression("a"))
    );

    CHECK_NE(
        ReturnStatement(new IdentExpression("a")),
        ReturnStatement(new IdentExpression("b"))
    );

    CHECK_NE(
        ReturnStatement(new IdentExpression("a")),
        ReturnStatement(new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("block statements equal") {
    CHECK_EQ(
        BlockStatement({}),
        BlockStatement({})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        BlockStatement({})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        BlockStatement({new ReturnStatement(new IdentExpression("b"))})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        BlockStatement({new BlockStatement({})})
    );
}

TEST_CASE("if statements equal") {
    CHECK_EQ(
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({})),
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({})),
        IfStatement(new IdentExpression("bar"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({})),
        IfStatement(new LiteralExpression(TokenType::INT, "100"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({})),
        IfStatement(new IdentExpression("foo"), NULL, new BlockStatement({new BlockStatement({})}))
    );

    CHECK_EQ(
        IfStatement(new IdentExpression("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({})),
        IfStatement(new IdentExpression("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(new IdentExpression("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({})),
        IfStatement(new IdentExpression("foo"), new IfStatement(NULL, NULL, new BlockStatement({new BlockStatement({})})), new BlockStatement({}))
    );
}

TEST_CASE("for statements equal") {
    CHECK_EQ(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("b"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("b"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::SUB_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({})
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new IdentExpression("foo"),
            new AssignStatement(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(new IdentExpression("a")),
            })
        )
    );
}

TEST_CASE("assignment statements equal") {
    CHECK_EQ(
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        AssignStatement(
            new IdentExpression("bar"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::SUB_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "200")
        )
    );

    CHECK_NE(
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new IdentExpression("bar")
        )
    );
}

TEST_CASE("functions equal") {
    CHECK_EQ(
        Function(
            "foo",
            {},
            {},
            new BlockStatement({})
        ),
        Function(
            "foo",
            {},
            {},
            new BlockStatement({})
        )
    );

    CHECK_NE(
        Function(
            "foo",
            {},
            {},
            new BlockStatement({})
        ),
        Function(
            "bar",
            {},
            {},
            new BlockStatement({})
        )
    );

    CHECK_EQ(
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        ),
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        )
    );

    CHECK_NE(
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "bar"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        ),
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        )
    );

    CHECK_NE(
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        ),
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::INT), "boo"),
            },
            new BlockStatement({})
        )
    );

    CHECK_NE(
        Function(
            "foo",
            {},
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        ),
        Function(
            "foo",
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bar"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "baz"),
            },
            {
                std::make_tuple(new PrimitiveType(Primitive::INT), "bing"),
                std::make_tuple(new PrimitiveType(Primitive::FLOAT), "boo"),
            },
            new BlockStatement({})
        )
    );

}

TEST_SUITE_END();