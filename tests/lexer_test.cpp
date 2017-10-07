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
        REQUIRE_EQ(TokenType::TOKEN_IDENT, tokens[0].type);
        REQUIRE_EQ(c, tokens[0].value);
    }
}

TEST_CASE("numbers") {
    auto cases = {
        std::make_tuple("0", TokenType::LITERAL_INT, "0"),
        std::make_tuple("1", TokenType::LITERAL_INT, "1"),
        std::make_tuple("1204", TokenType::LITERAL_INT, "1204"),
        std::make_tuple("213.42", TokenType::LITERAL_FLOAT, "213.42"),
        std::make_tuple("0.5", TokenType::LITERAL_FLOAT, ".5"),
        std::make_tuple("0x1000", TokenType::LITERAL_HEX, "1000"),
        std::make_tuple("0600", TokenType::LITERAL_OCTAL, "600"),
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
        std::make_tuple("\"test\"", TokenType::LITERAL_STRING, "test"),
        std::make_tuple("\"\"", TokenType::LITERAL_STRING, ""),
        std::make_tuple("\"\n\"", TokenType::LITERAL_STRING, "\n"),
        std::make_tuple("\"\021\"", TokenType::LITERAL_STRING, "\021"),
        std::make_tuple("\"\x41\"", TokenType::LITERAL_STRING, "\x41"),
        std::make_tuple("\"\u1000\"", TokenType::LITERAL_STRING, "\u1000"),
        std::make_tuple("\"\u10001000\"", TokenType::LITERAL_STRING, "\u10001000"),
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
        std::make_tuple(":", TokenType::SYMBOL_COLON, "" ),
        std::make_tuple(":=", TokenType::SYMBOL_DEFINE, "" ),
        std::make_tuple("::", TokenType::SYMBOL_DOUBLE_COLON, "" ),

        std::make_tuple(".", TokenType::SYMBOL_PERIOD, "" ),
        std::make_tuple("...", TokenType::SYMBOL_ELLIPSE, "" ),

        std::make_tuple(",", TokenType::SYMBOL_COMMA, "" ),

        std::make_tuple("(", TokenType::SYMBOL_LPAREN, "" ),
        std::make_tuple(")", TokenType::SYMBOL_RPAREN, "" ),
        std::make_tuple("[", TokenType::SYMBOL_LBRACK, "" ),
        std::make_tuple("]", TokenType::SYMBOL_RBRACK, "" ),
        std::make_tuple("{", TokenType::SYMBOL_LBRACE, "" ),
        std::make_tuple("}", TokenType::SYMBOL_RBRACE, "" ),

        std::make_tuple("+", TokenType::SYMBOL_ADD, "" ),
        std::make_tuple("+=", TokenType::SYMBOL_ADD_ASSIGN, "" ),
        std::make_tuple("++", TokenType::SYMBOL_INC, "" ),

        std::make_tuple("-", TokenType::SYMBOL_SUB, "" ),
        std::make_tuple("-=", TokenType::SYMBOL_SUB_ASSIGN, "" ),
        std::make_tuple("--", TokenType::SYMBOL_DEC, "" ),
        std::make_tuple("->", TokenType::SYMBOL_ARROW, "" ),

        std::make_tuple("*", TokenType::SYMBOL_MUL, "" ),
        std::make_tuple("*=", TokenType::SYMBOL_MUL_ASSIGN, "" ),

        std::make_tuple("/", TokenType::SYMBOL_QUO, "" ),
        std::make_tuple("/=", TokenType::SYMBOL_QUO_ASSIGN, "" ),

        std::make_tuple("%", TokenType::SYMBOL_REM, "" ),
        std::make_tuple("%=", TokenType::SYMBOL_REM_ASSIGN, "" ),

        std::make_tuple("^", TokenType::SYMBOL_XOR, "" ),
        std::make_tuple("^=", TokenType::SYMBOL_XOR_ASSIGN, "" ),

        std::make_tuple("<", TokenType::SYMBOL_LSS, "" ),
        std::make_tuple("<=", TokenType::SYMBOL_LEQ, "" ),
        std::make_tuple("<<", TokenType::SYMBOL_SHL, "" ),
        std::make_tuple("<<=", TokenType::SYMBOL_SHL_ASSIGN, "" ),

        std::make_tuple(">", TokenType::SYMBOL_GTR, "" ),
        std::make_tuple(">=", TokenType::SYMBOL_GEQ, "" ),
        std::make_tuple(">>", TokenType::SYMBOL_SHR, "" ),
        std::make_tuple(">>=", TokenType::SYMBOL_SHR_ASSIGN, "" ),

        std::make_tuple("=", TokenType::SYMBOL_ASSIGN, "" ),
        std::make_tuple("==", TokenType::SYMBOL_EQL, "" ),

        std::make_tuple("!", TokenType::SYMBOL_NOT, "" ),
        std::make_tuple("!=", TokenType::SYMBOL_NEQ, "" ),

        std::make_tuple("&", TokenType::SYMBOL_AND, "" ),
        std::make_tuple("&=", TokenType::SYMBOL_AND_ASSIGN, "" ),
        std::make_tuple("&&", TokenType::SYMBOL_LAND, "" ),
        std::make_tuple("&^", TokenType::SYMBOL_AND_NOT, "" ),
        std::make_tuple("&^=", TokenType::SYMBOL_AND_NOT_ASSIGN, "" ),
    
        std::make_tuple("|", TokenType::SYMBOL_OR, ""),
        std::make_tuple("||", TokenType::SYMBOL_LOR, ""),
        std::make_tuple("|=", TokenType::SYMBOL_OR_ASSIGN, ""),
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
        std::make_tuple("break", TokenType::KEY_WORD_BREAK),
        std::make_tuple("case", TokenType::KEY_WORD_CASE),
        std::make_tuple("const", TokenType::KEY_WORD_CONST),
        std::make_tuple("continue", TokenType::KEY_WORD_CONTINUE),
        std::make_tuple("default", TokenType::KEY_WORD_DEFAULT),
        std::make_tuple("defer", TokenType::KEY_WORD_DEFER),
        std::make_tuple("else", TokenType::KEY_WORD_ELSE),
        std::make_tuple("fallthrough", TokenType::KEY_WORD_FALLTHROUGH),
        std::make_tuple("for", TokenType::KEY_WORD_FOR),
        std::make_tuple("func", TokenType::KEY_WORD_FUNC),
        std::make_tuple("proc", TokenType::KEY_WORD_PROC),
        std::make_tuple("if", TokenType::KEY_WORD_IF),
        std::make_tuple("import", TokenType::KEY_WORD_IMPORT),
        std::make_tuple("return", TokenType::KEY_WORD_RETURN),
        std::make_tuple("select", TokenType::KEY_WORD_SELECT),
        std::make_tuple("struct", TokenType::KEY_WORD_STRUCT),
        std::make_tuple("switch", TokenType::KEY_WORD_SWITCH),
        std::make_tuple("type", TokenType::KEY_WORD_TYPE),
        std::make_tuple("var", TokenType::KEY_WORD_VAR),
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
                tokens[i+1].type != TokenType::SYMBOL_SEMI &&
                tokens[i+1].type != TokenType::SYMBOL_INC) os << " ";
        }

        std::string formatted = os.str(); 

        std::string expected = std::get<1>(c);
        REQUIRE_EQ(formatted, expected);
    }
}

TEST_SUITE_END();
