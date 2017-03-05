#include "stdafx.h"
#include "CppUnitTest.h"
#include "../FurlangC/lexer.c"
#include <stdlib.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FurlangCTest
{		
	TEST_CLASS(LexerTest)
	{
	public:
		struct tcase {
			char *input;
			TokenType expectedType;
			char *expectedValue;
		};
		
		TEST_METHOD(Identifier)
		{
			tcase cases[2] = {
				tcase{"test", IDENT, "test"},
				tcase{"a", IDENT, "a" },
			};

			for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
				tcase c = cases[i];

				Token *tokens = Lex(c.input);

				Assert::AreEqual((int)c.expectedType, (int)tokens[0].type);
				Assert::IsTrue(strcmp(c.expectedValue, tokens[0].value) == 0);
				Assert::AreEqual((int)END, (int)tokens[1].type);
			}
		}

		TEST_METHOD(IntegerNumbers) {
			tcase cases[] = {
				tcase{ "1", INT, "1" },
				tcase{ "1204", INT, "1204" },

				tcase{ "213.42", FLOAT, "213.42"},
				tcase{ "0.5", FLOAT, ".5" },
				
				tcase{"0x1000", HEX, "1000"},
				tcase{"0600", OCTAL, "600"},
			};

			for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
				tcase c = cases[i];

				Token *tokens = Lex(c.input);

				Assert::AreEqual((int)c.expectedType, (int)tokens[0].type);
				Assert::IsTrue(strcmp(c.expectedValue, tokens[0].value) == 0);
				Assert::AreEqual((int)END, (int)tokens[1].type);
			}
		}

		TEST_METHOD(Strings) {
			tcase cases[] = {
				tcase{ "\"test\"", STRING, "test" },
				tcase{ "\"\"", STRING, "" },

				tcase{ "\"\n\"", STRING, "\n" },
				tcase{ "\"\021\"", STRING, "\021" },
				tcase{ "\"\x41\"", STRING, "\x41" },
				tcase{ "\"\u1000\"", STRING, "\u1000" },
				tcase{ "\"\u10001000\"", STRING, "\u10001000" },
			};

			for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
				tcase c = cases[i];

				Token *tokens = Lex(c.input);

				Assert::AreEqual((int)c.expectedType, (int)tokens[0].type);
				Assert::IsTrue(strcmp(c.expectedValue, tokens[0].value) == 0);
				Assert::AreEqual((int)END, (int)tokens[1].type);
			}
		}

		TEST_METHOD(Symbols) {
			tcase cases[] = {
				tcase{ ":", COLON, "" },
				tcase{ ":=", DEFINE, "" },
				tcase{ "::", DOUBLE_COLON, "" },

				tcase{ ".", PERIOD, "" },
				tcase{ "...", ELLIPSE, "" },

				tcase{ ",", COMMA, "" },

				tcase{ "(", LPAREN, "" },
				tcase{ ")", RPAREN, "" },
				tcase{ "[", LBRACK, "" },
				tcase{ "]", RBRACK, "" },
				tcase{ "{", LBRACE, "" },
				tcase{ "}", RBRACE, "" },

				tcase{ "+", ADD, "" },
				tcase{ "+=", ADD_ASSIGN, "" },
				tcase{ "++", INC, "" },

				tcase{ "-", SUB, "" },
				tcase{ "-=", SUB_ASSIGN, "" },
				tcase{ "--", DEC, "" },
				tcase{ "->", ARROW, "" },

				tcase{ "*", MUL, "" },
				tcase{ "*=", MUL_ASSIGN, "" },

				tcase{ "/", QUO, "" },
				tcase{ "/=", QUO_ASSIGN, "" },

				tcase{ "%", REM, "" },
				tcase{ "%=", REM_ASSIGN, "" },

				tcase{ "^", XOR, "" },
				tcase{ "^=", XOR_ASSIGN, "" },

				tcase{ "<", LSS, "" },
				tcase{ "<=", LEQ, "" },
				tcase{ "<<", SHL, "" },
				tcase{ "<<=", SHL_ASSIGN, "" },

				tcase{ ">", GTR, "" },
				tcase{ ">=", GEQ, "" },
				tcase{ ">>", SHR, "" },
				tcase{ ">>=", SHR_ASSIGN, "" },

				tcase{ "=", ASSIGN, "" },
				tcase{ "==", EQL, "" },

				tcase{ "!", NOT, "" },
				tcase{ "!=", NEQ, "" },

				tcase{ "&", AND, "" },
				tcase{ "&=", AND_ASSIGN, "" },
				tcase{ "&&", LAND, "" },
				tcase{ "&^", AND_NOT, "" },
				tcase{ "&^=", AND_NOT_ASSIGN, "" },
			
				tcase{"|", OR, ""},
				tcase{"||", LOR, ""},
				tcase{"|=", OR_ASSIGN, ""},
			};

			for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
				tcase c = cases[i];

				Token *tokens = Lex(c.input);

				Assert::AreEqual((int)c.expectedType, (int)tokens[0].type);
				Assert::IsTrue(strcmp(c.expectedValue, tokens[0].value) == 0);
				Assert::AreEqual((int)END, (int)tokens[1].type);
			}
		}

		TEST_METHOD(Lines) {
			Token *tokens = Lex("1\n2\n3");
			
			for (int i = 0; i < 3; i++) {
				Assert::AreEqual(i+1, tokens[i].line);	
			}
		}

		TEST_METHOD(Columns) {
			Token *tokens = Lex("foo bar baz");

			Assert::AreEqual(1, tokens[0].column);
			Assert::AreEqual(5, tokens[1].column);
			Assert::AreEqual(9, tokens[2].column);
		}
	};
}