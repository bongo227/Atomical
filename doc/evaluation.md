## Evaluation
In the analysis I stated that "simple algorithms like the greatest common divisor, bubble sort and Fibonacci sequence should be able to be made in Fur. Each of these algorithms are included as part of the integrations tests, which all pass, so I would say the final program meets the original requirements.

| Objective | Comment |
| --- | --- |
| It must parse functions definitions such as `proc add :: int a, int b -> int` | Function definitions are correctly parsed as shown in __ParserTest.ParseFunctionDefinition__ |
| It must parse standard form declaration statements such as `var int foo = 100` | Long form varible declarations are parsed correctly as shown in __ParserTest.ParseLongVaribleDeclare__ |
| It must parse type inference declaration statements such as `foo := 100` | Short form declaration statements are parsed correctly as shown in __ParserTest.ParseShortVaribleDeclare__ |
| It must parse assignment statements such as `foo = 123` | Assignment statements are parsed correctly as shown in __ParserTest.ParseAssignmentOperator__ |
| It must parse binary statements such as `a + b` | Binary statements are parsed correctly as shown in __ParserTest.ParseBinaryExpression__ |
| It must parse return statements such as `return foo` | Return statements are parsed correctly as shown in __ParserTest.ParseReturnStatment__ |
| Block statements such as `{ foo := 100 }` | Block statements are parsed correctly as shown in __ParserTest.ParseBlockStatment__ |
| If statements such as `if foo > 100 {}` | If statements are parsed correctly as shown in __ParserTest.ParseIfStatment__ |
| For statements such as `for i := 0; i < 100; i++ {}` | For statements are parsed correctly as shown in __ParserTest.ParseForLoop__ |
| It must parse literals such as `123` | Literal expressions are parsed correctly as shown in __ParserTest.ParseLiteralExpression__ |
| It must parse unary operators such as `-foo` | Unary operators are parsed correctly as shown in __ParserTest.ParseUnaryExpression__ and __ParserTest.ParseUnaryMinusExpression__ |
| It must parse index expressions such as `foo[0]` | Index expressions are parsed correctly as shown in __ParserTest.ParseIndexExpression__ |
| It must parse call expressions such as `foo(1, 2)` | Call expressions are parsed correctly as shown in __ParserTest.ParseCallExpression__ |
| It must parse array expressions such as `{1, 2, 3}` | Array expressions are parsed correctly as shown in __ParserTest.ParseArrayExpression__ |
| It must parse array types such as `int[4]` | Array types are parsed correctly as shown in __ParserTest.ParseArrayType__ |
| Compile programs should have no runtime managed memory | The compiled application has no runtime, so their cannot be any runtime managed memory |
| Create an executable that invokes the compiler | As seen in my build system the compiler is compiled into a library _furlib_ and the application _fur_ which links against that library. |
| It must parse the `--output`, `--tokens` and `--ircode` flags | These flags are parsed correctly and modify the behavior of the compiler. These flags where very helpfull in debugging the compiler during development | 

### Feedback
> Ben sent me his project to review, and I was most impressed with the features we was able to develop. I first executed his test suite which show all 120+ tests passing successfully, demonstrating that each unit of the project works as intended and compiles all the example programs correctly.
>
> Next I tried to compile some of my own programs. I was impressed with how _neat_ the code looked, the lack of semicolons and brackets made the syntax fun to write. While full programs could not yet be made in Furlang, I see no reason this could not be developed into a fully featured programming language.
>
> The improvements for this kind of project are endless due to the nature of it. In terms of the language a module system and standard library would allow more advance application to be created within Furlang. Currently the compiler only works on linux, it would be nice to see windows and mac builds, with options to build for any platform. A website and some documentation is realy important for projects like this, to gain the attraction of more users.
>
>-- Theo, a colleague of Ben

If I had more time I would have liked to add all of these features. A module system is essential for any non-trivial program, so that code can be broken up across several files. The implementation of a module system would also present an interesting opportunity to parralize the compiler. First a dependency tree would be constructed from what each file includes, then for every included file an individual instance of the compiler would compile them to an AST. From their the AST's can be mergendinto a single tree which would then be lowered into LLVM IR.

With a module system in place the implementation of a standard library would be the next feature to implement. When the compiler reaches an include/import statement it would first search the standard library folder before the working directory. All the standard library would be implemented in Fur (and eventually the compiler itself) however the language would have to mature a bit more before this became viable, for example their is currently no way to interact with the OS. Things like printing and writing to files require diffrent implementations for every OS, which in turn requires a more complex build system to compile the write procedure/module for every target.

The implmentation of a good standard library brings alot of challanges in terms of interfacing with the operating system, so at the same time as a building a module system, I would like to abstract many of the operating system specific sections so that it would become viable to port the compiler to many diffrent platforms. Again the are quite alot of challenges, since the project is written in C, the language make no guarantees in terms of the ability for the executable to function correclty on diffrent platforms. To ensure correctly functioning builds I would need access to many diffrent hardware configurations for local debugging.

Of course no one will use a language without good documentation, and a website to demo its features. Websites of this nature tend to have live demos which are compiled and executed on a server, with the result being sent back to your browser. To do this I would need to setup a server which links against the furlib library to compile user generated code. Of course their are alot of security risks to manage, seeing as you will be executing someone elses (possible malicious) code. 