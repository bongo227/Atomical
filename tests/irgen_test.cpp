TEST_CASE("Gen Test") {
	{
		Irgen irgen = Irgen("");
		BasicBlock block = irgen.gen(std::vector<Statement *>{
			Statement::Return(
				Expression::Binary(
					TokenType::ADD,
					Expression::Literal(TokenType::INT, "100"),
					Expression::Literal(TokenType::INT, "23")))
		});

		CHECK_EQ(block.to_string(), "b1:\n"
									"\tv1 = 100 + 23\n"
									"\tret v1\n");
	}

	{
		Irgen irgen = Irgen("");
		BasicBlock block = irgen.gen(std::vector<Statement *>{
			Statement::Return(
				Expression::Unary(
					TokenType::SUB,
					Expression::Literal(TokenType::INT, "123")))
		});

		CHECK_EQ(block.to_string(), "b1:\n"
									"\tv1 = -123\n"
									"\tret v1\n");
	}

	{
		Irgen irgen = Irgen("");
		BasicBlock block = irgen.gen(std::vector<Statement *>{
			Statement::Return(
				Expression::Binary(
					TokenType::ADD,
					Expression::Binary(
						TokenType::SUB,
						Expression::Literal(TokenType::INT, "1"),
						Expression::Literal(TokenType::INT, "2")
					),
					Expression::Literal(TokenType::INT, "3")
				)
			)
		});

		CHECK_EQ(block.to_string(), "b1:\n"
									"\tv1 = 1 - 2\n"
									"\tv2 = v1 + 3\n"
									"\tret v2\n");
	}

	{
		Irgen irgen = Irgen("proc gcd :: int a, int b -> int c { return 123; }");

		CHECK_EQ(irgen.to_string(), "proc gcd :: int a, int b -> int c:\n"
									"b1:\n"
									"\tret 123\n\n");
	}

	// binaryFloat.acl
	{
		Irgen irgen = Irgen("proc main :: -> int c { return 130.75 - 7.75; }");

		CHECK_EQ(irgen.to_string(), "proc main :: -> int c:\n"
									"b1:\n"
									"\tv1 = 130.75 - 7.75\n"
									"\tret v1\n\n");
	}

	// binaryInt.acl
	{
		Irgen irgen = Irgen("proc main :: -> int c { return 120 + 3;  }");

		// CHECK_EQ(irgen.to_string(), "proc main :: -> int c:\n"
		//                             "b1:\n"
		//                             "\tv1 = 120 + 3\n"
		//                             "\tret v1\n\n");

		CHECK_EQ(irgen.to_string(), 
		R"(proc main :: -> int c:
b1:
	v1 = 120 + 3
	ret v1

)");
	}


	// {
	//     // Irgen irgen = Irgen("proc main :: -> int c { return 120 + 3;  }");

	//     CHECK_EQ(irgen.to_string(), "proc main :: -> int c:\n"
	//                                 "b1:\n"
	//                                 "\tv1 = 120 + 3\n"
	//                                 "\tret v1\n\n");
	// }
}