TEST_SUITE_BEGIN("Lexer");

TEST_CASE("whitespace is ignored") {
    Lexer *lexer = new Lexer("   \t\n\r\n  ");
    std::deque<Token> tokens = lexer->lex();
    REQUIRE_EQ(0, tokens.size());
}

TEST_CASE("identifiers") {
    auto cases = {"foo", "a", "bar100"};

    for (auto c : cases) {
        Lexer *lexer = new Lexer(c);
        std::deque<Token> tokens = lexer->lex();
        REQUIRE_EQ(2, tokens.size());
        REQUIRE_EQ(TokenType::IDENT, tokens[0].type);
        REQUIRE_EQ(c, tokens[0].value);
    }
}

TEST_CASE("numbers") {
    auto cases = {
        std::make_tuple("0", TokenType::INT, "0"),
        std::make_tuple("1", TokenType::INT, "1"),
        std::make_tuple("1204", TokenType::INT, "1204"),
        std::make_tuple("213.42", TokenType::FLOAT, "213.42"),
        std::make_tuple("0.5", TokenType::FLOAT, ".5"),
        std::make_tuple("0x1000", TokenType::HEX, "1000"),
        std::make_tuple("0600", TokenType::OCTAL, "600"),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        std::deque<Token> tokens = lexer->lex();
        REQUIRE_EQ(2, tokens.size());
        REQUIRE_EQ(std::get<1>(c), tokens[0].type);
        REQUIRE_EQ(std::get<2>(c), tokens[0].value);    
    }
}

TEST_CASE("strings") {
    auto cases = {
        std::make_tuple("\"test\"", TokenType::STRING, "test"),
        std::make_tuple("\"\"", TokenType::STRING, ""),
        std::make_tuple("\"\n\"", TokenType::STRING, "\n"),
        std::make_tuple("\"\021\"", TokenType::STRING, "\021"),
        std::make_tuple("\"\x41\"", TokenType::STRING, "\x41"),
        std::make_tuple("\"\u1000\"", TokenType::STRING, "\u1000"),
        std::make_tuple("\"\u10001000\"", TokenType::STRING, "\u10001000"),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        auto tokens = lexer->lex();
        REQUIRE_EQ(2, tokens.size());
        REQUIRE_EQ(std::get<1>(c), tokens[0].type);
        REQUIRE_EQ(std::get<2>(c), tokens[0].value);
    }
}

TEST_CASE("symbols") {
    auto cases = {
        std::make_tuple(":", TokenType::COLON, "" ),
        std::make_tuple(":=", TokenType::DEFINE, "" ),
        std::make_tuple("::", TokenType::DOUBLE_COLON, "" ),

        std::make_tuple(".", TokenType::PERIOD, "" ),
        std::make_tuple("...", TokenType::ELLIPSE, "" ),

        std::make_tuple(",", TokenType::COMMA, "" ),

        std::make_tuple("(", TokenType::LPAREN, "" ),
        std::make_tuple(")", TokenType::RPAREN, "" ),
        std::make_tuple("[", TokenType::LBRACK, "" ),
        std::make_tuple("]", TokenType::RBRACK, "" ),
        std::make_tuple("{", TokenType::LBRACE, "" ),
        std::make_tuple("}", TokenType::RBRACE, "" ),

        std::make_tuple("+", TokenType::ADD, "" ),
        std::make_tuple("+=", TokenType::ADD_ASSIGN, "" ),
        std::make_tuple("++", TokenType::INC, "" ),

        std::make_tuple("-", TokenType::SUB, "" ),
        std::make_tuple("-=", TokenType::SUB_ASSIGN, "" ),
        std::make_tuple("--", TokenType::DEC, "" ),
        std::make_tuple("->", TokenType::ARROW, "" ),

        std::make_tuple("*", TokenType::MUL, "" ),
        std::make_tuple("*=", TokenType::MUL_ASSIGN, "" ),

        std::make_tuple("/", TokenType::QUO, "" ),
        std::make_tuple("/=", TokenType::QUO_ASSIGN, "" ),

        std::make_tuple("%", TokenType::REM, "" ),
        std::make_tuple("%=", TokenType::REM_ASSIGN, "" ),

        std::make_tuple("^", TokenType::XOR, "" ),
        std::make_tuple("^=", TokenType::XOR_ASSIGN, "" ),

        std::make_tuple("<", TokenType::LSS, "" ),
        std::make_tuple("<=", TokenType::LEQ, "" ),
        std::make_tuple("<<", TokenType::SHL, "" ),
        std::make_tuple("<<=", TokenType::SHL_ASSIGN, "" ),

        std::make_tuple(">", TokenType::GTR, "" ),
        std::make_tuple(">=", TokenType::GEQ, "" ),
        std::make_tuple(">>", TokenType::SHR, "" ),
        std::make_tuple(">>=", TokenType::SHR_ASSIGN, "" ),

        std::make_tuple("=", TokenType::ASSIGN, "" ),
        std::make_tuple("==", TokenType::EQL, "" ),

        std::make_tuple("!", TokenType::NOT, "" ),
        std::make_tuple("!=", TokenType::NEQ, "" ),

        std::make_tuple("&", TokenType::AND, "" ),
        std::make_tuple("&=", TokenType::AND_ASSIGN, "" ),
        std::make_tuple("&&", TokenType::LAND, "" ),
        std::make_tuple("&^", TokenType::AND_NOT, "" ),
        std::make_tuple("&^=", TokenType::AND_NOT_ASSIGN, "" ),
    
        std::make_tuple("|", TokenType::OR, ""),
        std::make_tuple("||", TokenType::LOR, ""),
        std::make_tuple("|=", TokenType::OR_ASSIGN, ""),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        std::deque<Token> tokens = lexer->lex();
        // REQUIRE_EQ(1, tokens.size());
        REQUIRE_EQ(std::get<1>(c), tokens.front().type);
        REQUIRE_EQ(std::get<2>(c), tokens.front().value);
    }
}

TEST_CASE("line numbers") {
    Lexer *lexer = new Lexer("1\n2\n3");
    std::deque<Token> tokens = lexer->lex();
    REQUIRE_EQ(6, tokens.size());
    for (int i = 0; i < 6; i+=2) {
        REQUIRE_EQ(i/2+1, tokens[i].line);
    }
}

TEST_CASE("column numbers") {
    Lexer *lexer = new Lexer("foo bar baz");
    std::deque<Token> tokens = lexer->lex();
 
    REQUIRE_EQ(4, tokens.size());
    REQUIRE_EQ(1, tokens[0].column);
    REQUIRE_EQ(5, tokens[1].column);
    REQUIRE_EQ(9, tokens[2].column);
}

TEST_CASE("keywords") {
    auto cases = {
        std::make_tuple("break", TokenType::BREAK),
        std::make_tuple("case", TokenType::CASE),
        std::make_tuple("const", TokenType::CONST),
        std::make_tuple("continue", TokenType::CONTINUE),
        std::make_tuple("default", TokenType::DEFAULT),
        std::make_tuple("defer", TokenType::DEFER),
        std::make_tuple("else", TokenType::ELSE),
        std::make_tuple("fallthrough", TokenType::FALLTHROUGH),
        std::make_tuple("for", TokenType::FOR),
        std::make_tuple("func", TokenType::FUNC),
        std::make_tuple("proc", TokenType::PROC),
        std::make_tuple("if", TokenType::IF),
        std::make_tuple("import", TokenType::IMPORT),
        std::make_tuple("return", TokenType::RETURN),
        std::make_tuple("select", TokenType::SELECT),
        std::make_tuple("struct", TokenType::STRUCT),
        std::make_tuple("switch", TokenType::SWITCH),
        std::make_tuple("type", TokenType::TYPE),
        std::make_tuple("var", TokenType::VAR),
    };

    for (auto c : cases) {
        Lexer *lexer = new Lexer(std::get<0>(c));
        std::deque<Token> tokens = lexer->lex();
        REQUIRE_EQ(2, tokens.size());
        REQUIRE_EQ(std::get<1>(c), tokens[0].type);
    }
}

TEST_CASE("semicolon insertion") {
    auto cases = {
        std::make_tuple("{ }", "{ };"),
        std::make_tuple("{ }\n", "{ };"),
        std::make_tuple("{ a := 0 }", "{ a := 0; };"),
        std::make_tuple("{ a := 0; b := 0 }", "{ a := 0; b := 0; };"),
        std::make_tuple("{ a := 0\n b := 0 }", "{ a := 0; b := 0; };"),
        std::make_tuple("{ for i := 0; i < 123; i++ { } }", "{ for i := 0; i < 123; i++ { }; };"),
        std::make_tuple("if foo { }", "if foo { };"),
        std::make_tuple("if foo { } else { }", "if foo { } else { };"),
        std::make_tuple("for a := 0; a < 20; a += 1 { }", "for a := 0; a < 20; a += 1 { };"),
        std::make_tuple("a\n\n", "a;"),
    };

    for (auto c : cases) {
        std::deque<Token> tokens = Lexer(std::get<0>(c)).lex();
        std::ostringstream os;
        for (int i = 0; i < tokens.size(); i++) {
            os << tokens[i];
            if (i < tokens.size() - 1 && 
                tokens[i+1].type != TokenType::SEMI &&
                tokens[i+1].type != TokenType::INC) os << " ";
        }

        std::string formatted = os.str(); 

        std::string expected = std::get<1>(c);
        REQUIRE_EQ(formatted, expected);
    }
}

TEST_SUITE_END();
