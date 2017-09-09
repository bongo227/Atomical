TEST_SUITE_BEGIN("Ast");

TEST_CASE("identifier expressions equal") {
    REQUIRE_EQ(
        *Expression::Ident("foo"), 
        *Expression::Ident("foo")
    );
    
    REQUIRE_NE(
        *Expression::Ident("foo"), 
        *Expression::Ident("bar")
    );
}

TEST_CASE("literal expressions equal") {
    REQUIRE_EQ(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::INT, "100")
    );
    
    REQUIRE_NE(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::FLOAT, "100")
    );

    REQUIRE_NE(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::INT, "200")
    );
}

TEST_CASE("unary expressions equal") {
    REQUIRE_EQ(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")), 
        *Expression::Unary(TokenType::NOT, Expression::Ident("a"))
    );
    
    REQUIRE_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::NOT, Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::SUB, Expression::Ident("a"))
    );

    REQUIRE_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::NOT, Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("binary expressions equal") {
    REQUIRE_EQ(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::SUB, Expression::Ident("a"), Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("b"), Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("a"))
    );

    REQUIRE_NE(
        *Expression::Binary(TokenType::ADD, 
            Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, 
            Expression::Literal(TokenType::INT, "100"), Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), 
            Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), 
            Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("call expressions equal") {
    REQUIRE_EQ(
        *Expression::Call("foo", {}),
        *Expression::Call("foo", {})
    );

    REQUIRE_NE(
        *Expression::Call("foo", {}),
        *Expression::Call("bar", {})
    );

    REQUIRE_EQ(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Ident("a")})
    );

    REQUIRE_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {})
    );

    REQUIRE_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Ident("b")})
    );

    REQUIRE_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Literal(TokenType::INT, "100")})
    );
}

TEST_CASE("return statements equal") {
    REQUIRE_EQ(
        *Statement::Return(Expression::Ident("a")),
        *Statement::Return(Expression::Ident("a"))
    );

    REQUIRE_NE(
        *Statement::Return(Expression::Ident("a")),
        *Statement::Return(Expression::Ident("b"))
    );

    REQUIRE_NE(
        *Statement::Return(Expression::Ident("a")),
        *Statement::Return(Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("block statements equal") {
    REQUIRE_EQ(
        *Statement::Block({}),
        *Statement::Block({})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(Expression::Ident("a"))}),
        *Statement::Block({})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(Expression::Ident("a"))}),
        *Statement::Block({Statement::Return(Expression::Ident("b"))})
    );

    REQUIRE_NE(
        *Statement::Block({Statement::Return(Expression::Ident("a"))}),
        *Statement::Block({Statement::Block({})})
    );
}

TEST_CASE("if statements equal") {
    REQUIRE_EQ(
        *Statement::If(Expression::Ident("foo"), Statement::Block({}), NULL),
        *Statement::If(Expression::Ident("foo"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(Expression::Ident("foo"), Statement::Block({}), NULL),
        *Statement::If(Expression::Ident("bar"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(Expression::Ident("foo"), Statement::Block({}), NULL),
        *Statement::If(Expression::Literal(TokenType::INT, "100"), Statement::Block({}), NULL)
    );

    REQUIRE_NE(
        *Statement::If(Expression::Ident("foo"), Statement::Block({}), NULL),
        *Statement::If(Expression::Ident("foo"), Statement::Block({Statement::Block({})}), NULL)
    );

    REQUIRE_EQ(
        *Statement::If(
            Expression::Ident("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        ),
        *Statement::If(
            Expression::Ident("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        )
    );

    REQUIRE_NE(
        *Statement::If(
            Expression::Ident("foo"),
            Statement::Block({}),
            Statement::If(NULL, Statement::Block({}), NULL)
        ),
        *Statement::If(
            Expression::Ident("foo"),
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
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("b"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("b"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::SUB_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({})
        )
    );

    REQUIRE_NE(
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        ),
        *Statement::For(
            Statement::Assign(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Ident("foo"),
            Statement::Assign(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            Statement::Block({
                Statement::Return(Expression::Ident("a")),
            })
        )
    );
}

TEST_CASE("assignment statements equal") {
    REQUIRE_EQ(
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        *Statement::Assign(
            Expression::Ident("bar"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::SUB_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "200")
        )
    );

    REQUIRE_NE(
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        *Statement::Assign(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Ident("bar")
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
                    Expression::Binary(
                        TokenType::ADD,
                        Expression::Ident("a"),
                        Expression::Literal(TokenType::FLOAT, "u")
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
                    Expression::Binary(
                        TokenType::ADD,
                        Expression::Ident("a"),
                        Expression::Literal(TokenType::FLOAT, "1")
                    )
                )
            })
        )
    );

}

TEST_SUITE_END();