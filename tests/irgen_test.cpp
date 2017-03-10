#include <llvm-c/Core.h>

TEST(IrgenTest, CompileTypes) {
    struct tcase {
        char *src;
        LLVMTypeRef expectedType;
    };

    tcase cases[] = {
        {"int", LLVMInt64Type()},
        {"i64", LLVMInt64Type()},
        {"i32", LLVMInt32Type()},
        {"i16", LLVMInt16Type()},
        {"i8", LLVMInt8Type()},

        {"float", LLVMFloatType()},
        {"f64", LLVMDoubleType()},
        {"f32", LLVMFloatType()},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Parser *parser = NewParser(Lex(c.src));
        Exp *e = ParseType(parser);
        Irgen *irgen = NewIrgen();
        LLVMTypeRef type = CompileType(e);

        ASSERT_TRUE(type == c.expectedType);
    }
}

TEST(IrgenTest, CompileLiterals) {
    struct tcase {
        char *src;
        LLVMTypeRef expectedType;
        char *expectedValue;
    };

    tcase cases[] = {
        {"123", LLVMInt64Type(), "i64 123"},
        {"123.321", LLVMFloatType(), "float 0x405ED48B40000000"},
        {"0x1000", LLVMInt64Type(), "i64 4096"},
        {"0123", LLVMInt64Type(), "i64 83"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Parser *parser = NewParser(Lex(c.src));
        Exp *e = ParseExpression(parser, 0);
        Irgen *irgen = NewIrgen();
        LLVMValueRef value = CompileLiteral(irgen, e);

        ASSERT_TRUE(LLVMIsConstant(value));
        ASSERT_STREQ(c.expectedValue, LLVMPrintValueToString(value));
    }
}

// creates an empty function for use in unit tests
Irgen *createTestFunction() {
    Irgen *irgen = NewIrgen();
    
    irgen->function = LLVMAddFunction(irgen->module, "test", LLVMInt64Type());
    
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
    irgen->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(irgen->builder, entry);
    
    return irgen;
}

TEST(IrgenTest, CastValues) {
    struct tcase {
        LLVMValueRef value;
        LLVMTypeRef cast;
    };

    tcase cases[] = {
        { LLVMConstIntOfString(LLVMInt64Type(), "123", 10), LLVMInt32Type() },
        { LLVMConstIntOfString(LLVMInt16Type(), "32", 10), LLVMFloatType() },
        { LLVMConstRealOfString(LLVMFloatType(), "1.42"), LLVMDoubleType() },
        { LLVMConstRealOfString(LLVMDoubleType(), "1245.12"), LLVMInt64Type() },
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Irgen *irgen = createTestFunction();
        LLVMValueRef value = Cast(irgen, c.value, c.cast);

        ASSERT_EQ(LLVMTypeOf(value), c.cast);
    }
}