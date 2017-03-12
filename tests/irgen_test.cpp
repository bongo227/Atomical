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

        {"[3]int", LLVMArrayType(LLVMInt64Type(), 3)},
        {"[100]float", LLVMArrayType(LLVMFloatType(), 100)},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];
        log("Compiling %s type", c.src);

        Parser *parser = NewParser(c.src, Lex(c.src));
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

        Parser *parser = NewParser(c.src, Lex(c.src));
        Exp *e = ParseExpression(parser, 0);
        Irgen *irgen = NewIrgen();
        LLVMValueRef value = CompileLiteralExp(irgen, e);

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

LLVMGenericValueRef intArg(int n) {
    return LLVMCreateGenericValueOfInt(LLVMInt64Type(), n, false);
}

LLVMGenericValueRef runLLVMFunction(
    Irgen *irgen, 
    LLVMValueRef function, 
    int paramCount, 
    LLVMGenericValueRef *params) {

    // create an execution engine
    LLVMExecutionEngineRef engine;
    char *error = NULL;

    # if 0
        // initialize jit
        LLVMLinkInMCJIT();
    # else
        // Initialize intepreter
        LLVMLinkInInterpreter();
    # endif

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    assert(LLVMCreateExecutionEngineForModule(&engine, irgen->module, &error) == 0);
    assert(error == NULL);

    LLVMGenericValueRef res = LLVMRunFunction(engine, function, paramCount, params);

    LLVMDisposeExecutionEngine(engine);
    return res;
}


TEST(IrgenTest, FunctionTests) {
    struct tcase {
        char *src;    
        LLVMGenericValueRef params[3];
        int out;
    };
    
    tcase cases[] = {
        {
            "proc test :: -> int { return 123 }",
            { }, 123,
        },

        {
            "proc test :: -> int { return 121 + 2 }",
            { }, 123,
        },

        {
            "proc test :: -> int { return 130.75 - 7.75 }",
            { }, 123,
        },

        {
            "proc test :: int a, int b -> int { return a + b }",
            { intArg(100), intArg(23) }, 123,
        },

        {
            "proc test :: int n -> int { return -n }",
            { intArg(-123) }, 123,
        },

        {
            "proc test :: int n -> int { n = 123; return n }",
            { intArg(321) }, 123,
        },

        {
            "proc test :: -> int { var int a = 123; return a }",
            { }, 123,
        },

        {
            "proc test :: -> int { a := 123; return a }",
            { }, 123,
        },

        {
            "proc test :: -> int { if true { return 123 }; return 321 }",
            { }, 123,
        },

        {
            "proc test :: -> int { if true { return 123 } else { return 321 } }",
            { }, 123,
        },

        {
            "proc test :: -> int { if false { return 321 } else if true { return 123 } else { return 0 } }",
            { }, 123,
        },

        {
            "proc test :: -> int { if false { return 321 } else if false { return 23 } else if false { return 21; } else { return 123 } }",
            { }, 123,
        },

        {
            "proc test :: -> int { a := 0; for i := 0; i < 123; i++ { a += 1 }; return a }",
            { }, 123,
        },

        // {
        //     "proc test :: -> int { a := [3]int{0, 123, 321}; return a[1] }",
        //     { }, 123,
        // },
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) { 
        tcase c = cases[i];
        log("testing function \"%s\"", c.src);

        int paramCount = 0;
        while(c.params[paramCount] != NULL) paramCount++;

        // generate function
        Parser *parser = NewParser(c.src, Lex(c.src));
        Dcl *d = ParseFunction(parser);
        Irgen *irgen = NewIrgen();
        LLVMValueRef function = CompileFunction(irgen, d);
        
        // LLVMDumpModule(irgen->module);

        // check for errors in module
        char *error = NULL;
        LLVMVerifyModule(irgen->module, LLVMPrintMessageAction, &error);
        LLVMDisposeMessage(error);

        // run the function
        LLVMGenericValueRef res = runLLVMFunction(irgen, function, paramCount, c.params);
        ASSERT_EQ((int)LLVMGenericValueToInt(res, 0), c.out);

        // dispose of builder
        LLVMDisposeBuilder(irgen->builder);
    }
}

TEST(IrgenTest, CallTest) {
    char *src = "proc add :: int a, int b -> int { return a + b }\n"
                "proc test :: -> int { return add(120, 3) }";

    Parser *parser = NewParser(src, Lex(src));
    Dcl *addDcl = ParseFunction(parser);
    Dcl *testDcl = ParseFunction(parser);
    
    Irgen *irgen = NewIrgen();
    LLVMValueRef addFunction = CompileFunction(irgen, addDcl);
    LLVMValueRef testFunction = CompileFunction(irgen, testDcl);

    // LLVMDumpModule(irgen->module);

    // check for errors in module
    char *error = NULL;
    LLVMVerifyModule(irgen->module, LLVMPrintMessageAction, &error);
    LLVMDisposeMessage(error);

    // run the function
    LLVMGenericValueRef res = runLLVMFunction(irgen, testFunction, 0, NULL);
    ASSERT_EQ((int)LLVMGenericValueToInt(res, 0), 123);

    // dispose of builder
    LLVMDisposeBuilder(irgen->builder);
}

