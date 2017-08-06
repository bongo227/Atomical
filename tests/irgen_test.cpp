TEST_CASE("Gen Test") {
    {
        Irgen irgen = Irgen("");
        Instruction *i = irgen.gen(Statement::Return(
            Expression::Binary(
                TokenType::ADD,
                Expression::Literal(TokenType::INT, "100"),
                Expression::Literal(TokenType::INT, "23"))
        ));

        std::ostringstream stream;
        for (auto i : irgen.currentBlock) {
            stream << *i << std::endl;
        }
        std::string str = stream.str();
        
        CHECK_EQ(str, "v1 = 100 + 23\n"
                      "ret v1\n");
    }

    {
        Irgen irgen = Irgen("");
        Instruction *i = irgen.gen(Statement::Return(
            Expression::Unary(
                TokenType::SUB,
                Expression::Literal(TokenType::INT, "123"))
        ));

        std::ostringstream stream;
        for (auto i : irgen.currentBlock) {
            stream << *i << std::endl;
        }
        std::string str = stream.str();
        
        CHECK_EQ(str, "v1 = -123\n"
                      "ret v1\n");
    }
}