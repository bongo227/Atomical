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
		TEST_METHOD(ScopeEnter) {
			Parser *parser = NewParser(NULL);
			Scope *outer = parser->scope;
			Assert::IsNull(outer->outer);
			EnterScope(parser);
			Assert::IsTrue(outer == parser->scope->outer);
		}

		TEST_METHOD(ScopeExit) {
			Parser *parser = NewParser(NULL);
			Scope *outer = parser->scope;
			EnterScope(parser);
			ExitScope(parser);
			Assert::IsTrue(outer == parser->scope);
		}

		TEST_METHOD(ScopeInsert) {
			Parser *parser = NewParser(NULL);
			Object *obj = (Object *)malloc(sizeof(Object));
			obj->type = badObj;
			obj->name = "test";
			obj->node = NULL;
			obj->typeInfo = NULL;
			InsertScope(parser, "test", obj);

			ScopeObject *found;
			HASH_FIND_STR(parser->scope->objects, "test", found);
			Assert::AreEqual(obj->name, found->obj->name);			
		}

		TEST_METHOD(ScopeFind) {
			Parser *parser = NewParser(NULL);
			Object *obj = (Object *)malloc(sizeof(Object));
			obj->type = badObj;
			obj->name = "test";
			obj->node = NULL;
			obj->typeInfo = NULL;
			InsertScope(parser, "test", obj);

			// Enter and exit some scopes
			EnterScope(parser);
			EnterScope(parser);
			ExitScope(parser);
			EnterScope(parser);
			ExitScope(parser);

			Object *found = FindScope(parser, "test");
			Assert::AreSame(obj->name, found->name);
		}

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
			char *src = "{\nreturn test\n}";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)blockSmt, (int)smt->type);
			Assert::AreEqual(1, smt->node.block.count);
			Assert::AreEqual((int)returnSmt, (int)smt->node.block.smts->type);
		}

		TEST_METHOD(ParseIfStatment) {
			char *src = "if true {\nreturn false\n}";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)ifSmt, (int)smt->type);
			Assert::AreEqual((int)identExp, (int)smt->node.ifs.cond->type);
			Assert::AreEqual((int)blockSmt, (int)smt->node.ifs.body->type);
			Assert::IsNull(smt->node.ifs.elses);
		}

		TEST_METHOD(ParserShortVaribleDeclare) {
			char *src = "a := 10";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)declareSmt, (int)smt->type);
			Assert::AreEqual((int)varibleDcl, (int)smt->node.declare->type);

			Object *obj = FindScope(parser, "a");
			Assert::IsTrue(obj->node == smt->node.declare);
		}

		TEST_METHOD(ParseLongVaribleDeclare) {
			char *src = "var int a = 10";
			Parser *parser = NewParser(Lex(src));
			Smt *smt = ParseStatement(parser);

			Assert::AreEqual((int)declareSmt, (int)smt->type);
			Assert::AreEqual((int)varibleDcl, (int)smt->node.declare->type);
			Assert::AreEqual((int)identExp, (int)smt->node.declare->node.varible.name->type);
			Assert::AreEqual("int", smt->node.declare->node.varible.type->node.ident.name);

			Object *obj = FindScope(parser, "a");
			Assert::IsNotNull(obj);
			Assert::IsTrue(obj->node == smt->node.declare);
		}

		TEST_METHOD(ParseArrayType) {
			char *src = "int[3]";
			Parser *parser = NewParser(Lex(src));
			Exp *type = ParseType(parser);

			Assert::AreEqual((int)arrayTypeExp, (int)type->type);
		}
	};
}