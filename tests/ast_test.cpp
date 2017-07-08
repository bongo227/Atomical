TEST_SUITE_BEGIN("Ast");

TEST_CASE("identifier expressions equal") {
    CHECK_EQ(
        *Expression::Ident("foo"), 
        *Expression::Ident("foo")
    );
    
    CHECK_NE(
        *Expression::Ident("foo"), 
        *Expression::Ident("bar")
    );
}

TEST_CASE("literal expressions equal") {
    CHECK_EQ(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::INT, "100")
    );
    
    CHECK_NE(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::FLOAT, "100")
    );

    CHECK_NE(
        *Expression::Literal(TokenType::INT, "100"), 
        *Expression::Literal(TokenType::INT, "200")
    );
}

TEST_CASE("unary expressions equal") {
    CHECK_EQ(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")), 
        *Expression::Unary(TokenType::NOT, Expression::Ident("a"))
    );
    
    CHECK_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::NOT, Expression::Ident("b"))
    );

    CHECK_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::SUB, Expression::Ident("a"))
    );

    CHECK_NE(
        *Expression::Unary(TokenType::NOT, Expression::Ident("a")),
        *Expression::Unary(TokenType::NOT, Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("binary expressions equal") {
    CHECK_EQ(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b"))
    );

    CHECK_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::SUB, Expression::Ident("a"), Expression::Ident("b"))
    );

    CHECK_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("b"), Expression::Ident("b"))
    );

    CHECK_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("a"))
    );

    CHECK_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Literal(TokenType::INT, "100"), Expression::Ident("b"))
    );

    CHECK_NE(
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Ident("b")),
        *Expression::Binary(TokenType::ADD, Expression::Ident("a"), Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("call expressions equal") {
    CHECK_EQ(
        *Expression::Call("foo", {}),
        *Expression::Call("foo", {})
    );

    CHECK_NE(
        *Expression::Call("foo", {}),
        *Expression::Call("bar", {})
    );

    CHECK_EQ(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Ident("a")})
    );

    CHECK_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {})
    );

    CHECK_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Ident("b")})
    );

    CHECK_NE(
        *Expression::Call("foo", {Expression::Ident("a")}),
        *Expression::Call("foo", {Expression::Literal(TokenType::INT, "100")})
    );
}

TEST_CASE("return statements equal") {
    CHECK_EQ(
        ReturnStatement(Expression::Ident("a")),
        ReturnStatement(Expression::Ident("a"))
    );

    CHECK_NE(
        ReturnStatement(Expression::Ident("a")),
        ReturnStatement(Expression::Ident("b"))
    );

    CHECK_NE(
        ReturnStatement(Expression::Ident("a")),
        ReturnStatement(Expression::Literal(TokenType::INT, "100"))
    );
}

TEST_CASE("block statements equal") {
    CHECK_EQ(
        BlockStatement({}),
        BlockStatement({})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(Expression::Ident("a"))}),
        BlockStatement({})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(Expression::Ident("a"))}),
        BlockStatement({new ReturnStatement(Expression::Ident("b"))})
    );

    CHECK_NE(
        BlockStatement({new ReturnStatement(Expression::Ident("a"))}),
        BlockStatement({new BlockStatement({})})
    );
}

TEST_CASE("if statements equal") {
    CHECK_EQ(
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({})),
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({})),
        IfStatement(Expression::Ident("bar"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({})),
        IfStatement(Expression::Literal(TokenType::INT, "100"), NULL, new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({})),
        IfStatement(Expression::Ident("foo"), NULL, new BlockStatement({new BlockStatement({})}))
    );

    CHECK_EQ(
        IfStatement(Expression::Ident("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({})),
        IfStatement(Expression::Ident("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({}))
    );

    CHECK_NE(
        IfStatement(Expression::Ident("foo"), new IfStatement(NULL, NULL, new BlockStatement({})), new BlockStatement({})),
        IfStatement(Expression::Ident("foo"), new IfStatement(NULL, NULL, new BlockStatement({new BlockStatement({})})), new BlockStatement({}))
    );
}

TEST_CASE("for statements equal") {
    CHECK_EQ(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("b"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("b"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::SUB_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({})
        )
    );

    CHECK_NE(
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Binary(
                TokenType::LSS,
                Expression::Ident("a"),
                Expression::Literal(TokenType::INT, "10")
            ),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        ),
        ForStatement(
            new AssignStatement(
                Expression::Ident("a"), 
                TokenType::DEFINE, 
                Expression::Literal(TokenType::INT, "0")
            ),
            Expression::Ident("foo"),
            new AssignStatement(
                Expression::Ident("a"),
                TokenType::ADD_ASSIGN,
                Expression::Literal(TokenType::INT, "1")
            ),
            new BlockStatement({
                new ReturnStatement(Expression::Ident("a")),
            })
        )
    );
}

TEST_CASE("assignment statements equal") {
    CHECK_EQ(
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        AssignStatement(
            Expression::Ident("bar"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::SUB_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        )
    );

    CHECK_NE(
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "200")
        )
    );

    CHECK_NE(
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Literal(TokenType::INT, "100")
        ),
        AssignStatement(
            Expression::Ident("foo"),
            TokenType::ADD_ASSIGN,
            Expression::Ident("bar")
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

    CHECK_NE(
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
            new BlockStatement({
                new ReturnStatement(
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