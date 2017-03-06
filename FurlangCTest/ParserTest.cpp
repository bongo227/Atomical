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
		TEST_METHOD(ParseIdentExpression) {
			char *src = "a";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)identExp, (int)exp->type);
		}

		TEST_METHOD(ParseBinaryExpression) {
			char *src = "a + b";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)binaryExp, (int)exp->type);
		}

		TEST_METHOD(ParseBidmasBinaryExpression) {
			char *src = "a + b * c";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)binaryExp, (int)exp->type);
			Assert::AreEqual((int)ADD, (int)exp->node.binary.op.type);
			Assert::AreEqual((int)MUL, (int)exp->node.binary.right->node.binary.op.type);
		}

		TEST_METHOD(ParseSelectorExpression) {
			char *src = "a.b";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)selectorExp, (int)exp->type);
			Assert::AreEqual("a", exp->node.selector.exp->node.ident.name);
			Assert::AreEqual("b", exp->node.selector.selector->node.ident.name);
		}

		TEST_METHOD(ParseDoubleSelectorExpression) {
			char *src = "a.b.c";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)selectorExp, (int)exp->type);
			Assert::AreEqual((int)selectorExp, (int)exp->node.selector.exp->type);
			Assert::AreEqual("a", exp->node.selector.exp->node.selector.exp->node.ident.name);
			Assert::AreEqual("b", exp->node.selector.exp->node.selector.selector->node.ident.name);
			Assert::AreEqual("c", exp->node.selector.selector->node.ident.name);
		}

		TEST_METHOD(ParseIndexExpression) {
			char *src = "test[1]";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)indexExp, (int)exp->type);
		}

		TEST_METHOD(ParseRightAssociativeBinaryOperators) {
			char *src = "a || b || c";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)binaryExp, (int)exp->type);
			Assert::AreEqual((int)binaryExp, (int)exp->node.binary.right->type);
		}

		TEST_METHOD(ParseUnaryExpression) {
			char *src = "!a";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);
			
			Assert::AreEqual((int)unaryExp, (int)exp->type);
		}

		TEST_METHOD(ParseUnaryMinuxExpression) {
			char *src = "-a";
			Parser *parser = NewParser(Lex(src));
			Exp *exp = ParseExpression(parser, 0);

			Assert::AreEqual((int)unaryExp, (int)exp->type);
		}

		TEST_METHOD(ParseAssignmentOperator) {
			char *src = "a = b";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)assignmentSmt, (int)smt->type);
		}

		TEST_METHOD(ParseAddAssigmentOperator) {
			char *src = "a += b";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)assignmentSmt, (int)smt->type);
			Assert::AreEqual((int)binaryExp, (int)smt->node.assignment.right->type);
			Assert::AreEqual((int)ADD, (int)smt->node.assignment.right->node.binary.op.type);
			Assert::AreEqual("a", smt->node.assignment.right->node.binary.left->node.ident.name);
			Assert::AreEqual("b", smt->node.assignment.right->node.binary.right->node.ident.name);
		}

		TEST_METHOD(ParseReturnStatment) {
			char *src = "return a";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)returnSmt, (int)smt->type);
		}

		TEST_METHOD(ParseBlockStatment) {
			// TODO: fix parser not inserting semis
			char *src = "{\nreturn test;\n}";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)blockSmt, (int)smt->type);
			Assert::AreEqual(1, smt->node.block.count);
			Assert::AreEqual((int)returnSmt, (int)smt->node.block.smts->type);
		}

		TEST_METHOD(ParseIfStatment) {
			char *src = "if true {\nreturn false;\n}";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)ifSmt, (int)smt->type);
			Assert::AreEqual((int)identExp, (int)smt->node.ifs.cond->type);
			Assert::AreEqual((int)blockSmt, (int)smt->node.ifs.body->type);
			Assert::IsNull(smt->node.ifs.elses);
		}
	};
}