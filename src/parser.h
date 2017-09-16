class Parser {
    std::deque<Token> tokens;
    
    public:
        explicit Parser(std::deque<Token>);
        explicit Parser(std::string source);
      
        std::vector<Function *> parse(); // TODO: consider removing pointer
      
        Function *parse_function();
        Statement *parse_statement();
        Expression *parse_expression(int rbp);
        

    private:
        Token expect(TokenType type);
        void accept(TokenType type);
          
        ReturnStatement *parse_return_statement();
        BlockStatement *parse_block_statement();
        IfStatement *parse_if_statement();
        ForStatement *parse_for_statement();
        AssignStatement *parse_assign_statement();

        Expression *nud(Token token);
        Expression *led(Token token, Expression *expression);
        
        Type *parse_type();
};