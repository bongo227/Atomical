class Irgen {
    std::vector<Function *> functions;
    
    public:
        explicit Irgen(std::vector<Function *> functions);
        explicit Irgen(std::string source);

    private:
        //
};

Irgen::Irgen(std::vector<Function *> functions) : functions(functions) {}

Irgen::Irgen(std::string source) : Irgen(Parser(source).parse()) {}

