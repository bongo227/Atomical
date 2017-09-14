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
        *Statement::Return(new IdentExpression("a")),
        *Statement::Return(new IdentExpression("a"))
    );

    REQUIRE_NE(
        *Statement::Return(new IdentExpression("a")),
        *Statement::Return(new IdentExpression("b"))
    );

    REQUIRE_NE(
        *Statement::Return(new IdentExpression("a")),
        *Statement::Return(new LiteralExpression(TokenType::INT, "100"))
    );
}

TEST_CASE("block statements equal") {
    REQUIRE_EQ(
        *Statement::Block({}),
        *Statement::Block({})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(new IdentExpression("a"))}),
        *Statement::Block({})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(new IdentExpression("a"))}),
        *Statement::Block({Statement::Return(new IdentExpression("b"))})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(new IdentExpression("a"))}),
        *Statement::Block({Statement::Block({})})
    );
}

TEST_CASE("if statements equal") {
    REQUIRE_EQ(
        *Statement::If(new IdentExpression("foo"), Statement::Block({}), NULL),
        *Statement::If(new IdentExpression("foo"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(new IdentExpression("foo"), Statement::Block({}), NULL),
        *Statement::If(new IdentExpression("bar"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(new IdentExpression("foo"), Statement::Block({}), NULL),
        *Statement::If(new LiteralExpression(TokenType::INT, "100"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(new IdentExpression("foo"), Statement::Block({}), NULL),
        *Statement::If(new IdentExpression("foo"), Statement::Block({Statement::Block({})}), NULL)
    );

    REQUIRE_EQ(
        *Statement::If(
            new IdentExpression("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        ),
        *Statement::If(
            new IdentExpression("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        )
    );

    REQUIRE_NE(
        *Statement::If(
            new IdentExpression("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        ),
        *Statement::If(
            new IdentExpression("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({
                Statement::Block({})
            }), NULL)
        )
    );
}

TEST_CASE("for statements equal") {
    REQUIRE_EQ(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("b"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("b"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::SUB_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({})
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new BinaryExpression(
                TokenType::LSS,
                new IdentExpression("a"),
                new LiteralExpression(TokenType::INT, "10")
            ),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                new IdentExpression("a"), 
                TokenType::DEFINE, 
                new LiteralExpression(TokenType::INT, "0")
            ),
            new IdentExpression("foo"),
            Statement::Assign(
                new IdentExpression("a"),
                TokenType::ADD_ASSIGN,
                new LiteralExpression(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(new IdentExpression("a")),
            })
        )
    );
}

TEST_CASE("assignment statements equal") {
    REQUIRE_EQ(
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *Statement::Assign(
            new IdentExpression("bar"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::SUB_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "200")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            new IdentExpression("foo"),
            TokenType::ADD_ASSIGN,
            new LiteralExpression(TokenType::INT, "100")
        ),
        *Statement::Assign(
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
            Statement::Block({})
        ),
        Function(
            "foo",
            {},
            {},
            Statement::Block({})
        )
    );

    REQUIRE_NE(
        Function(
            "foo",
            {},
            {},
            Statement::Block({})
        ),
        Function(
            "bar",
            {},
            {},
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({})
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
            Statement::Block({
                Statement::Return(
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
            Statement::Block({
                Statement::Return(
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