#include "stdafx.h"
#include "CppUnitTest.h"
#include "../FurlangC/ast.c"
#include "../FurlangC/parser.c"
#include <stdlib.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FurlangCTest
{
	TEST_CLASS(ParserTest) {
	public:
		TEST_METHOD(ParseBinaryExpression) {
			char *src = "a + b";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)binaryExp, (int)exp->type);
		}
	};
}