# Furlang: An investigation into modern programming languages and compilers

## Analysis

In this investigation the aim is to design a programming language and implement a compiler to create executable programs. Due to the time constraints it would be infeasible to implement all aspects of a modern programming language, standard library and language tooling. Instead the focus will be on implementing a sizeable subset such that simple algorithums like the greatest common devisor, bubble sort and insertion sort can be created.

### Background
The first recognizable modern computers were created in the 1940's. Their limited hardware meant programmers would write hand tuned assembly which had none of the abstraction of modern languages meaning programs were slow to develop and error-prone. Autocode, developed in the 1950's, was the first higher-level compiled language. The invention of a compiler meant programmers could write less code and rely on the compiler to make optermizations that had previously required a large amount of knowledge to implement.

Fortran was invented in 1954 at IBM, it was the first higher level functional language. To this day fortran can be seen in high performace code bases espesically in the acedemic areas of computer science like machine learning. List is another language developed towards the enfo of the 1950's that is still in use today. Lisp brougt lexical scoping which allowed a block to hide its varibles and procedures from code outside that block.

The 1960's - 70's saw the invention of much of the syntax and paradigms used today. Simula was the first langauge to support object oriented programing were code is seperated into many objects which have methods and attributes much like real objects. ML brought the first staically typed functional programming languse with its polymorphic type system. C was developed at Bell labs and quickly became one of the most important languages ever, powering many low level systems to this day.

The 80's broght alot of refinement such as the idea of organizing code into modules which helped make large code bases more maintainable and generic programming which meant programmers could write functions that worked for many diffrent types rather than many functions for each possible type. Computer instrubtion sets got smaller and more efficent which lead way to many improvements in the speed of programs which meant more advance systems could be designed.

The explosive growth of the internet in the 90's caused widespread devleopment and use of scripting languages, espsicaly javascript which powers almost all of the interactive portions of websites to this day. Other developments included the integrated development enviroment which packaged all the languages tool into a text editor which made programming easier and faster. Garbage collection meant that memory no longer needed to be managed by the programmer, instead the runtime would detect when data was not being used and recycle for when the programmer needed new memory.

Since then concurrent constructs such as callbacks and langauge level scedulers for light weight threads meant that programs could more easily take advantage of a modern processor with many cores. The idea of open source where programmers would post the code online so anyone could use and improve the software. OpenCL and CUDA allowed programmers to write massively parralel software for the GPU.

### Research
In terms of the languages design I looked at several languages with similar goals as mine and read through their specifications including: [Rust<sup>[1]</sup>](#1), [Go<sup>[2]</sup>](#2), [D<sup>[3]</sup>](#3) and [F#<sup>[4]</sup>](#4). I looked at their syntax and the design disicions behind them in order the judge what code should look like.

### Syntax
Compared to C modern programming languages use a lot less characters to describe the instructions which make the final program. By using less character it becomes alot faster to read through the source code in order to understand the logic, which intern makes the language easier to use and faster to develop in. With Fur, I wanted to explore some of these modern ideas and how they can be implemented. 

#### Type inference
In go, most varibles dont need a type because their type can be infered:
```go
foo := 123
```
In this case foo will be an `int` type since the value on the right hand side is an integer. If you want to be more explicit you can use the longer form varible declaration:
```go
var foo int = 123
``` 
The infered type is much quicker to type and just as easy to read, healping to reduce the character count of the source code. 

#### Semicolons and parentiesis
Most languages use semi colons to tell the compiler that the current statement has ended and everyting after the semicolon is interpreted as a seperate statment. In more modern languages like python, their are no semicolons anywhere in the language. Instead python uses spaces and new lines to signify the start and end of each statement and block.

Go uses semicolons only in a few places such as for loops:
```go
for i := 0; i < 10; i++ {}
```
Here each part of the for loop is seperate by semicolons just like C, however for most statements new lines are used as in python to signify the end of a statement.

Another thing to note is the lack of parentiesis around the for loop. The lack of brackets helps to further eliminate useless syntax which only hinders the programmer. The side effect of ommiting semicolons and brackets is that the source code is much more complex to parse since the compiler must insert semicolons before the syntax can be parsed (or use some other method to deal with the lack of semicolons).

#### Function definitions
I decided to create my own syntax for function definitions inspired partly by functional programming languages. 
```
proc bar :: int foo -> float
```
The double semi colon is used to provide a clear divider between the name and the arguments, this clear line of seperation helps when skimming though the source code in order to find a function with a certain name. Also the arrow that seperates the arguments and return type reinforces the consept of a function, to take input and transform it into output.

#### Keywords
Golang has [25 keywords<sup>[2]</sup>](#2) which helps make it's easy/quick to learn, simple to read and less keywords are reserved so it's simpler to name some constructs. The obvious drawback is the reduced expressiveness compared to languages like C# and Java which have many more keywords. Fur will be closer to Go in terms of the small pool of keywords in favor of a more simpler and clearer expression of logic.

#### Symbols
Functional languages like F# tend to use far more symbols to express their logic. Symbols make smaller code and less visual noise however they create a steeper learning curve since they are less descriptive and often unique to each language. Fur should feel familiar, sharing most of the symbols from C based languages.

#### Operator overloading
C++ and Java both have operator overloading which makes their source code easy to read in code bases that use things like arbitrarily wide integers, vectors and matrices with complex mathmatical operations. The problem is operator overloading is easily abused by hiding complex computations behind a simple looking operator. For example in the case of a two arbitrary length integers being multiplied, memory allocations and complex loops are invoked which isn't obvious from the call site. To help to programmer reason about the performance and create fast efficent code fur will opt for no function overloading.

#### Syntax objectives
* Parse the definition syntax
  * Functions definitions such as `proc add :: int a, int b -> int`
* Parse the statement syntax
  * Declaration statements
    * In standard form such as `var int foo = 100`
    * With type inference such as `foo := 100`
  * Assignment statements such as `foo = 123`
  * Binary statements
    * `+` (addition operator)
    * `-` (subtraction operator)
    * `*` (multiplication operator)
    * `/` (division operator)
    * `%` (modulus operator)
    * `>` (greater than operator)
    * `>=` (greater than or equal to operator)
    * `<` (less than operator)
    * `<=` (less than or equal to operator)
  * Return statements such as `return foo`
  * Block statments such as `{ foo := 100 }`
  * If statments such as `if foo > 100 {}`
  * For statments such as `for i := 0; i < 100; i++ {}`
* Parse the expression syntax
  * Literal expressions
    * Integers such as `123`
    * Floats such as `13.41`
    * Octals such as `032`
    * Hexedecimals such as `0xffff`
  * Unary operators
    * `-` (minus operator)
  * Index expressions such as `foo[12]`
  * Call expressions such as `foo(1, 43)`
  * Array expressions such as `{1, 2, 3}`
  * Array types such as `int[4]`
  * Integer types:
    * `i8` (8 bit integer)
    * `i16` (16 bit integer)
    * `i32` (32 bit integer)
    * `i64` (64 bit integer)
    * `int` (platform specific fastest integer)
  * Float types:
    * `f32` (32 bit float)
    * `f64` (64 bit float)
    * `float` (platform specific fastest float)

### Memory managment
When a program needs memory to persist longer than the scope of a function, memory needs to be allocated from the heap. The heap is slower than stack but the program can choose at run-time how much memory it wants. This flexibility brings several problems such as: what if the operating system can't give you the memory you requested, what if you need more, what if the you never give it back. In languages with manual memory management the programmer must solve all these problems whenever they need to allocate memory on the heap, making the code more complex and error prone.

One solution to this problem is a garbage collector. In languages such as Go and Java the run-time allocates blocks of memory, whenever the program requests memory a portion of that block is returned. The run-time then keeps track of where the memory is used, when the portion of memory is no longer in use its is marked and recycled ready for the next allocation. Over time these garbage collectors have been getting faster and much more sophisticated, one consequence of this is that it can be hard to understand their behavior.

The problems arises in applications which have low latency requirements, such as games. With virtual reality and higher refresh rate monitors, games have less than 7 milliseconds to update and render the next frame. GC max pause times in Go are around [50µs<sup>[6]</sup>](#6) (with considerable CPU usage) and [50ms<sup>[7]</sup>](#7) in Java, what's worse is that they can happen at anytime causing the game to freeze and stutter. One workaround is to budget this into your frame time i.e. 5ms to update and render and 2ms for any GC pauses, this means reduced graphics, less realistic physics and simpler game mechanics. Even if you do not pause mid-frame there is still the problem of: higher read/write latency, less CPU performance and less data locality (hence less cache utilization). For these reason Fur will not have a garbage collector.

#### Memory managment objectives
 * Compile programs should have no runtime managed memory.

### Command line interface
Compilers normaly expose a command line interface to transform the syntax into an executable file. This makes it very easy to integrate the compiler with other build tools, text editors and IDE's. Fur doesn't require a large range of flags and options since it is a small language however some debug options could help the programmer (and the development of the compiler).

#### Command line interface objectives
* Create an executable that invokes the compiler
* `-o`, `--output` flag should control the path of the compiled executable
* `-t`, `--tokens` produces a file with a list of the tokens (for debugging)
* `-i`, `--ircode` produces a file with the LLVM IR code for the program (for debugging)  

## Documented design

### Lexer
The lexer's job is to turn the source code into a sequence of tokens. A token is the smallest possible bit of meaningfull source code such as a number or name. The following is all the possible types of token:

| Name | Example | Note |
| --- | --- | --- |
| ILLEGAL | | If a token isnt one of the other types of token it becomes an illegal token and somthing fately wrong has happened.
| END | | The last token, signifys the end of the token list. |
| IDENT | `foo` | Identifier |
| INT | `123` | Integer literal |
| FLOAT | `10.23` | Float literal |
| HEX | `0xFFFFFF` | Hex literal |
| OCTAL | `0123` | Octal literal |
| STRING | `"foo"` | String literal |
| BREAK | `break` | Break keyword |
| CASE | `case` | Case keyword |
| CONST | `const` | Const keyword |
| CONTINUE | `continue` | Continue keyword |
| DEFAULT | `default` | Default keyword |
| ELSE | `else` | Else keyword |
| FALLTHROUGH | `fallthrough` | Fallthrough keyword |
| FOR | `for` | For keyword |
| FUNC | `func` | Func keyword |
| PROC | `proc` | Proc keyword |
| IF | `if` | If keyword |
| IMPORT | `import` | Import keyword |
| RETURN | `return` | Return keyword |
| SELECT | `select` | Select keyword |
| STRUCT | `struct` | Struct keyword |
| SWITCH | `switch` | Switch keyword |
| TYPE | `type` | Type keyword |
| VAR | `var` | Var keyword |
| DEFER | `defer` | Defer keyword |
| DEFINE | `:=` | Define symbol |
| SEMI | `;` | Semicolon symbol |
| COLON | `:` | Colon symbol |
| DOUBLE_COLON | `::` | Double colon symbol |
| ELLIPSE | `...` | Ellipse symbol |
| PERIOD | `.` | Period symbol |
| COMMA | `,` | Comma symbol |
| LPAREN | `(` | Left parentesis symbol |
| RPAREN | `)` | Right parentesis symbol |
| LBRACK | `[` | Left bracket symbol |
| RBRACK | `]` | Right parentesis symbol |
| LBRACE | `{` | Left brace symbol |
| RBRACE | `}` | Right brace symbol |
| ADD | `+` | Add symbol |
| ADD_ASSIGN | `+=` | Add assign symbol |
| INC | `++` | Increment symbol |
| ARROW | `->` | Arrow symbol |
| SUB | `-` | Substract symbol |
| SUB_ASSIGN | `-=` | Subtract assign symbol |
| DEC | `--` | Decrement symbol |
| MUL | `*` | Multiplication symbol |
| MUL_ASSIGN | `*=` | Mutltiply assign symbol |
| QUO | `/` | Quotiant symbol |
| QUO_ASSIGN | `/=` | Quotiant assign symbol |
| REM | `%` | Remainder symbol |
| REM_ASSIGN | `%=` | Remainder assign symbol |
| XOR | `^` | Excusive or symbol |
| XOR_ASSIGN | `^=` | Excusive or assign symbol |
| GTR | `>` | Greater than symbol |
| GEQ | `>=` | Greater than or equal to symbol |
| LSS | `<` | Less than symbol |
| LEQ | `<=` | Less than or equal to symbol |
| SHL | `<<` | Shift left symbol |
| SHL_ASSIGN | `<<=` | Shift left assign symbol |
| SHR | `>>` | Shift right symbol |
| SHR_ASSIGN | `>>=` | Shift right assign symbol |
| ASSIGN | `=` | Assign symbol |
| EQL | `==` | Equality symbol |
| NOT | `!` | Not symbol |
| NEQ | `!=` | Not equal to symbol |
| AND | `&` | Bitwise and sybol |
| AND_ASSIGN | `&=` | Bitwise and assign symbol |
| AND_NOT | `&^` | Bit clear symbol |
| AND_NOT_ASSIGN | `&^=` | Bit clear assign symbol |
| LAND | `&&` | Logical and symbol |
| OR | `|` | Bitwise or symbol |
| OR_ASSIGN | `|=` | Bitwise or assign symbol |
| LOR | `|` | Logical or symbol |

To convert the source to token the lexer runs through the file character by character invoking diffrent procedures depending on which character the head points to.

#### Extracting a number
Fur has four types of numerical literals: ints, floats, octals and hexedecimals. Instead of a seperate procedure for each one which would require backtracking/look-head and alot of code duplication we use a single procedure. Note this uses gotos which are normaly a terrible idea, in this case they make the code more efficent, cleaner and easier to follow. 

The first part is `extractMantissa` which returns all the digits which are less than `base`. `isDigit` checks if a character literal is a digit (0-F) and `asDigit` converts a character literal to an integer. `character` is the current character the lexer is reading and `nextCharacter` moves the `character` pointer to the next character.  

```
FUNCTION extractMantissa(base)
  mantissa <- ""
  WHILE isDigit(character) && asDigit(character) < base THEN
    mantissa <- mantissa + character
    length <- length + 1
    nextCharacter()
  LOOP
  RETURN mantissa
END FUNCTION
```

This is the main part of the algortithum. If first checks for hexedecimals or octals then integers and floats, when we check for octals we may find that it was actualy an integer (or float) in which case we can jump to the appropriate point. 
```
FUNCTION number()
  number <- ""
  type <- INTEGER
  base <- 10

  IF character = '0' THEN
    nextCharacter()
    IF character = 'x' OR character = 'X' THEN
      type <- HEXEDECIMAL
      base <- 16
      nextCharacter()
      number <- number + extractMantissa(base)
    ELSE
      octal <- TRUE
      base <- 8
      number <- number + extractMantissa(base)

      IF character = '8' OR character = '9' THEN
        octal <- FALSE
        base <- 10
      ELSE
        type <- OCTAL
      END IF

      IF character = '.' THEN
        GOTO fraction
      END IF

      IF NOT octal THEN
        ERROR "Illegal octal number"
      END IF
    END IF

    GOTO exit
  END IF

  base <- 10
  number <- number + extractMantissa(base)

LABEL fraction
  IF character = '.' THEN
    number <- number + '.'
    nextCharacter()
    type <- FLOAT
    base <- 10
    number <- number + extractMantissa(base)
  END IF
END LABEL

LABEL exit
  RETURN number
END LABEL

END FUNCTION
```

## Parser
The parser takes the list of tokens constructed by the lexer and transforms them into an abstract syntax tree, which is a tree structure which represents the program being compiled. An AST is constructed from nodes each with children for example the expression `-a + 4 * 5` would be parsed into:
```golang
BinaryNode {
	Left: Unary Node {
		Operator: '-'
		Expression:  IdentNode{
			name: 'a'
		}
	}
	Operator: '+'
	Right: BinaryNode {
		Left: LiteralNode{
			value: 4
		}
		Operator: '*'
		Right: LiteralNode {
			value: 5
		}
	}
}
```
This structure allows us to more easily translate the higher level language into a lower level one.

### Pratt Parser
When it comes to language parsing their are many different options. A parser generator can automatically build a parser from a list of rules, however the error messages that they produce can be hard to understand and not very customizable. For this reason most languages opt to right their own parsers from scratch as I did with Fur. For this project I implemented a Top down operator precedence parser, also known as a Pratt parser, with a few modifications. 

The key parts of this algorithm is the `led` and `nud` procedures. `led` (short for left denotation) parses the next token in an infix context i.e. the token joins two expressions.

`nud` (short for null denotation) parser a token in a prefix context, i.e. the token starts a new expression.  
```
FUNCTION nud(token)
	SWITCH typeof(token)
		# Case statements here ...
	END SWITCH
END FUNCTION
```

#### Literal token
If a literal is at the beginning of an expression (or sub-expression) we return a literal node, any following operators will be handled in an infix context. This is a key distinction between some other algorithms, by delaying infix operators we are able to parse mathematical expressions like `a + b * c` without having to use something like the shunting yard algorithm which requires allocating a stack (two stacks for function calls) and queue.
```
CASE LITERAL:
	return LiteralNode { token }
```

#### Identifier token
```
CASE IDENT:
	return IdentNode { token }
```

#### Unary token
A unary node is somthing like `+100` or `-14`, the operator in front changes the value to the right. To parse a unary node we treat the tokens after the operator as a sub-expression by calling `expression()` (see below). This is what gives the ast its tree structure and allows us to parse expressions such as `-(14 + a)`, in this case the expression on the right wasnt a simple literal value but a binary expression.
```
CASE UNARY:
	return UnaryNode { token, expression() }
```

#### 

### Irgen.c
This file is the ir generation...

```c
#import "../src/irgen.c"
```

## References
1. The Rust Reference <a id="1">https://doc.rust-lang.org/reference.html</a>
2. The Go Programming Language Specification <a id="2">https://golang.org/ref/spec</a>
3. Specification for the D Programming Language <a id="3">https://dlang.org/spec/spec.html</a>
4. The F# Language Specification <a id="4">http://fsharp.org/specs/language-spec/</a>
5. Go's GitHub Repository <a id="5">https://github.com/golang/go</a>
6. Go - Proposal: Eliminate STW stack re-scanning <a id="6">https://golang.org/design/17503-eliminate-rescan</a>
7. Plumber - G1 vs CMS vs Parallel GC <a id="7">https://plumbr.eu/blog/garbage-collection/g1-vs-cms-vs-parallel-gc</a>
