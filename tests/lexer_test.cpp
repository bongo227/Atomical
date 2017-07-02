TEST(LexerTest, WhitespaceIsIgnored) {
    Lexer *lexer = new Lexer("   \t\n\r\n  ");
    std::vector<Token> tokens = lexer->lex();
    ASSERT_EQ(0, tokens.size());
}

TEST(LexerTest, Ident) {
    auto cases = {"foo", "a", "bar100"};

    for (auto c : cases) {
        Lexer *lexer = new Lexer(c);
        std::vector<Token> tokens = lexer->lex();
        ASSERT_EQ(1, tokens.size());
        ASSERT_EQ(TokenType::IDENT, tokens[0].type);
        ASSERT_EQ(c, tokens[0].value);
    }
}

TEST(LexerTest, Numbers) {
    auto cases = {
        std::make_tuple("1", INT, "1"),
        std::make_tuple("1204", INT, "1204"),
        std::make_tuple("213.42", FLOAT, "213.42"),
        std::make_tuple("0.5", FLOAT, ".5"),
        std::make_tuple("0x1000", HEX, "1000"),
        std::make_tuple("0600", OCTAL, "600"),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        std::vector<Token> tokens = lexer->lex();
        ASSERT_EQ(1, tokens.size());
        ASSERT_EQ(std::get<1>(c), tokens[0].type);
        ASSERT_EQ(std::get<2>(c), tokens[0].value);    
    }
}

TEST(LexerTest, Strings) {
    auto cases = {
        std::make_tuple("\"test\"", STRING, "test"),
        std::make_tuple("\"\"", STRING, ""),
        std::make_tuple("\"\n\"", STRING, "\n"),
        std::make_tuple("\"\021\"", STRING, "\021"),
        std::make_tuple("\"\x41\"", STRING, "\x41"),
        std::make_tuple("\"\u1000\"", STRING, "\u1000"),
        std::make_tuple("\"\u10001000\"", STRING, "\u10001000"),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        auto tokens = lexer->lex();
        ASSERT_EQ(1, tokens.size());
        ASSERT_EQ(std::get<1>(c), tokens[0].type);
        ASSERT_EQ(std::get<2>(c), tokens[0].value);
    }
}

TEST(LexerTest, Symbols) {
    auto cases = {
        std::make_tuple(":", COLON, "" ),
        std::make_tuple(":=", DEFINE, "" ),
        std::make_tuple("::", DOUBLE_COLON, "" ),

        std::make_tuple(".", PERIOD, "" ),
        std::make_tuple("...", ELLIPSE, "" ),

        std::make_tuple(",", COMMA, "" ),

        std::make_tuple("(", LPAREN, "" ),
        std::make_tuple(")", RPAREN, "" ),
        std::make_tuple("[", LBRACK, "" ),
        std::make_tuple("]", RBRACK, "" ),
        std::make_tuple("{", LBRACE, "" ),
        std::make_tuple("}", RBRACE, "" ),

        std::make_tuple("+", ADD, "" ),
        std::make_tuple("+=", ADD_ASSIGN, "" ),
        std::make_tuple("++", INC, "" ),

        std::make_tuple("-", SUB, "" ),
        std::make_tuple("-=", SUB_ASSIGN, "" ),
        std::make_tuple("--", DEC, "" ),
        std::make_tuple("->", ARROW, "" ),

        std::make_tuple("*", MUL, "" ),
        std::make_tuple("*=", MUL_ASSIGN, "" ),

        std::make_tuple("/", QUO, "" ),
        std::make_tuple("/=", QUO_ASSIGN, "" ),

        std::make_tuple("%", REM, "" ),
        std::make_tuple("%=", REM_ASSIGN, "" ),

        std::make_tuple("^", XOR, "" ),
        std::make_tuple("^=", XOR_ASSIGN, "" ),

        std::make_tuple("<", LSS, "" ),
        std::make_tuple("<=", LEQ, "" ),
        std::make_tuple("<<", SHL, "" ),
        std::make_tuple("<<=", SHL_ASSIGN, "" ),

        std::make_tuple(">", GTR, "" ),
        std::make_tuple(">=", GEQ, "" ),
        std::make_tuple(">>", SHR, "" ),
        std::make_tuple(">>=", SHR_ASSIGN, "" ),

        std::make_tuple("=", ASSIGN, "" ),
        std::make_tuple("==", EQL, "" ),

        std::make_tuple("!", NOT, "" ),
        std::make_tuple("!=", NEQ, "" ),

        std::make_tuple("&", AND, "" ),
        std::make_tuple("&=", AND_ASSIGN, "" ),
        std::make_tuple("&&", LAND, "" ),
        std::make_tuple("&^", AND_NOT, "" ),
        std::make_tuple("&^=", AND_NOT_ASSIGN, "" ),
    
        std::make_tuple("|", OR, ""),
        std::make_tuple("||", LOR, ""),
        std::make_tuple("|=", OR_ASSIGN, ""),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        std::vector<Token> tokens = lexer->lex();
        ASSERT_EQ(1, tokens.size());
        ASSERT_EQ(std::get<1>(c), tokens[0].type);
        ASSERT_EQ(std::get<2>(c), tokens[0].value);
    }
}

TEST(LexerTest, LineNumbers) {
    Lexer *lexer = new Lexer("1\n2\n3");
    std::vector<Token> tokens = lexer->lex();
    ASSERT_EQ(5, tokens.size());
    for (int i = 0; i < 0; i++) {
        ASSERT_EQ(i+1, tokens[i].line);
    }
}

TEST(LexerTest, ColumnNumbers) {
    Lexer *lexer = new Lexer("foo bar baz");
    std::vector<Token> tokens = lexer->lex();
 
    ASSERT_EQ(3, tokens.size());
    ASSERT_EQ(1, tokens[0].column);
    ASSERT_EQ(5, tokens[1].column);
    ASSERT_EQ(9, tokens[2].column);
}

TEST(LexerTest, SemiColonInsertion) {
    Lexer *lexer = new Lexer("foo\nbar");
    std::vector<Token> tokens = lexer->lex();

    ASSERT_EQ(3, tokens.size());
    ASSERT_EQ(SEMI, tokens[1].type);
}