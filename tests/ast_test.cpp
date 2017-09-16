TEST_SUITE_BEGIN("Ast");

TEST_CASE("identifier expressions equal") {
    REQUIRE_EQ(
        *new IdentExpression("foo"), 
        *new IdentExpression("foo")
    );
    
    REQUIRE_NE(
        *new IdentExpression("foo"), 
        *new IdentExpression("bar")
    );
}

TEST_CASE("literal expressions equal") {
    REQUIRE_EQ(
        *new LiteralExpression(TokenType::INT, "100"), 
        *new LiteralExpression(TokenType::INT, "100")
    );
    
    REQUIRE_NE(
        *new LiteralExpression(TokenType::INT, "100"), 
        *new LiteralExpression(TokenType::FLOAT, "100")
    );

    REQUIRE_NE(
        *new LiteralExpression(TokenType::INT, "100"), 
        *new LiteralExpression(TokenType::INT, "200")
    );
}

TEST_CASE("unary expressions equal") {
    REQUIRE_EQ(
        *new UnaryExpression(TokenType::NOT, new IdentExpression("a")), 
        *new UnaryExpression(TokenType::NOT, new IdentExpression("a"))
    );
    
    REQUIRE_NE(
        *new UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        *new UnaryExpression(TokenType::NOT, new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        *new UnaryExpression(TokenType::SUB, new IdentExpression("a"))
    );

    REQUIRE_NE(
        *new UnaryExpression(TokenType::NOT, new IdentExpression("a")),
        *new UnaryExpression(TokenType::NOT, new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("binary expressions equal") {
    REQUIRE_EQ(
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        *new BinaryExpression(TokenType::SUB, new IdentExpression("a"), new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        *new BinaryExpression(TokenType::ADD, new IdentExpression("b"), new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("b")),
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), new IdentExpression("a"))
    );

    REQUIRE_NE(
        *new BinaryExpression(TokenType::ADD, 
            new IdentExpression("a"), new IdentExpression("b")),
        *new BinaryExpression(TokenType::ADD, 
            new LiteralExpression(TokenType::INT, "100"), new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), 
            new IdentExpression("b")),
        *new BinaryExpression(TokenType::ADD, new IdentExpression("a"), 
            new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("call expressions equal") {
    REQUIRE_EQ(
        *new CallExpression("foo", {}),
        *new CallExpression("foo", {})
    );

    REQUIRE_NE(
        *new CallExpression("foo", {}),
        *new CallExpression("bar", {})
    );

    REQUIRE_EQ(
        *new CallExpression("foo", {new IdentExpression("a")}),
        *new CallExpression("foo", {new IdentExpression("a")})
    );

    REQUIRE_NE(
        *new CallExpression("foo", {new IdentExpression("a")}),
        *new CallExpression("foo", {})
    );

    REQUIRE_NE(
        *new CallExpression("foo", {new IdentExpression("a")}),
        *new CallExpression("foo", {new IdentExpression("b")})
    );

    REQUIRE_NE(
        *new CallExpression("foo", {new IdentExpression("a")}),
        *new CallExpression("foo", {new LiteralExpression(TokenType::INT, "100")})
    );
}

TEST_CASE("return statements equal") {
    REQUIRE_EQ(
        *new ReturnStatement(new IdentExpression("a")),
        *new ReturnStatement(new IdentExpression("a"))
    );

    REQUIRE_NE(
        *new ReturnStatement(new IdentExpression("a")),
        *new ReturnStatement(new IdentExpression("b"))
    );

    REQUIRE_NE(
        *new ReturnStatement(new IdentExpression("a")),
        *new ReturnStatement(new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("block statements equal") {
    REQUIRE_EQ(
        *new BlockStatement({}),
        *new BlockStatement({})
    );

    REQUIRE_NE(
        *new BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        *new BlockStatement({})
    );

    REQUIRE_NE(
        *new BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        *new BlockStatement({new ReturnStatement(new IdentExpression("b"))})
    );

    REQUIRE_NE(
        *new BlockStatement({new ReturnStatement(new IdentExpression("a"))}),
        *new BlockStatement({new BlockStatement({})})
    );
}

TEST_CASE("if statements equal") {
    REQUIRE_EQ(
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({}), NULL),
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({}), NULL)
    );

    REQUIRE_NE(
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({}), NULL),
        *new IfStatement(new IdentExpression("bar"), new BlockStatement({}), NULL)
    );

    REQUIRE_NE(
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({}), NULL),
        *new IfStatement(new LiteralExpression(TokenType::INT, "100"), new BlockStatement({}), NULL)
    );

    REQUIRE_NE(
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({}), NULL),
        *new IfStatement(new IdentExpression("foo"), new BlockStatement({new BlockStatement({})}), NULL)
    );

    REQUIRE_EQ(
        *new IfStatement(
            new IdentExpression("foo"),
            new BlockStatement({}),
            new IfStatement(NULL, new BlockStatement({}), NULL)
        ),
        *new IfStatement(
            new IdentExpression("foo"),
            new BlockStatement({}),
            new IfStatement(NULL, new BlockStatement({}), NULL)
        )
    );

    REQUIRE_NE(
        *new IfStatement(
            new IdentExpression("foo"),
            new BlockStatement({}),
            new IfStatement(NULL, new BlockStatement({}), NULL)
        ),
        *new IfStatement(
            new IdentExpression("foo"),
            new BlockStatement({}),
            new IfStatement(NULL, new BlockStatement({
                new BlockStatement({})
            }), NULL)
        )
    );
}

TEST_CASE("for statements equal") {
    REQUIRE_EQ(
        *new ForStatement(
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
        *new ForStatement(
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

    REQUIRE_NE(
        *new ForStatement(
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
        *new ForStatement(
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

    REQUIRE_NE(
        *new ForStatement(
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
        *new ForStatement(
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

    REQUIRE_NE(
        *new ForStatement(
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
        *new ForStatement(
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

    REQUIRE_NE(
        *new ForStatement(
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
        *new ForStatement(
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

    REQUIRE_NE(
        *new ForStatement(
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
        *new ForStatement(
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
    REQUIRE_EQ(
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *new AssignStatement(
            new IdentExpression("bar"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::SUB_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "200")
        )
    );

    REQUIRE_NE(
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *new AssignStatement(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new IdentExpression("bar")
        )
    );
}

TEST_CASE("functions equal") {
    REQUIRE_EQ(
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

    REQUIRE_NE(
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

    REQUIRE_EQ(
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

    REQUIRE_NE(
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

    REQUIRE_NE(
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

    REQUIRE_NE(
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

    REQUIRE_NE(
        Function(
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
                        new LiteralExpression(TokenType::FLOAT, "u")
                    )
                )
            })
        ),
        Function(
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
                        new LiteralExpression(TokenType::FLOAT, "1")
                    )
                )
            })
        )
    );

}

TEST_SUITE_END();