#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

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
        log("Compiling %s type", c.src);

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
        log("Compiling %s to %s", c.src, c.expectedValue);

        Parser *parser = NewParser(Lex(c.src));
        Exp *e = ParseExpression(parser, 0);
        Irgen *irgen = NewIrgen();
        LLVMValueRef value = CompileLiteral(irgen, e);

        ASSERT_TRUE(LLVMIsConstant(value));
        ASSERT_STREQ(c.expectedValue, LLVMPrintValueToString(value));
    }
}


TEST(IrgenTest, CastValues) {
    struct tcase {
        char *testName;
        LLVMValueRef value;
        LLVMTypeRef cast;
    };

    tcase cases[] = {
        { 
            "Cast i64 123 to i32", 
            LLVMConstIntOfString(LLVMInt64Type(), "123", 10), 
            LLVMInt32Type() 
        },
        { 
            "Cast i16 32 to float",
            LLVMConstIntOfString(LLVMInt16Type(), "32", 10), LLVMFloatType() 
        },
        { 
            "Cast float 1.42 to double",
            LLVMConstRealOfString(LLVMFloatType(), "1.42"), 
            LLVMDoubleType() 
        },
        {
            "Cast double 1245.12 to i64", 
            LLVMConstRealOfString(LLVMDoubleType(), "1245.12"), 
            LLVMInt64Type(),
        },
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];
        log(c.testName);

        Irgen *irgen = NewIrgen();
        irgen->function = LLVMAddFunction(irgen->module, "test", LLVMInt64Type());
        
        LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
        irgen->builder = LLVMCreateBuilder();
        LLVMPositionBuilderAtEnd(irgen->builder, entry);

        LLVMValueRef value = Cast(irgen, c.value, c.cast);

        ASSERT_EQ(LLVMTypeOf(value), c.cast);
    }
}

TEST(IrgenTest, FunctionTests) {
    char *src = "proc add :: int a, int b -> int { return 123 }";
    LLVMGenericValueRef params[] = { 
        LLVMCreateGenericValueOfInt(LLVMInt32Type(), 100, 0), 
        LLVMCreateGenericValueOfInt(LLVMInt32Type(), 23, 0),
    };
    int paramCount = sizeof(params) / sizeof(LLVMGenericValueRef);
    int out = 123;

    Parser *parser = NewParser(Lex(src));
    Dcl *d = ParseFunction(parser);
    Irgen *irgen = NewIrgen();
    LLVMValueRef function = CompileFunction(irgen, d);
    
    char *error = NULL;
    LLVMVerifyModule(irgen->module, LLVMPrintMessageAction, &error);
    LLVMDisposeMessage(error);

    // create an execution engine
    LLVMExecutionEngineRef engine;
    error = NULL;
    
    // initialize jit
    // LLVMLinkInMCJIT();
    // LLVMInitializeNativeTarget();
    // LLVMInitializeNativeAsmPrinter();
    // LLVMInitializeNativeAsmParser();
    
    // Initialize intepreter
    LLVMLinkInInterpreter();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    ASSERT_EQ(LLVMCreateExecutionEngineForModule(&engine, irgen->module, &error), 0);
    ASSERT_EQ(error, NULL);
    
    LLVMGenericValueRef res = LLVMRunFunction(engine, function, paramCount, params);
    ASSERT_EQ((int)LLVMGenericValueToInt(res, 0), 123);

    LLVMDisposeBuilder(irgen->builder);
    LLVMDisposeExecutionEngine(engine);
}