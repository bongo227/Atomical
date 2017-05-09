## Evaluation
In the analysis I stated that "simple algorithms like the greatest common divisor, bubble sort and Fibonacci sequence should be able to be made in Fur. Each of these algorithms are included as part of the integrations tests, which all pass, so I would say the final program meets the original requirements.

| Objective | Comment |
| --- | --- |
| Syntax objectives | As my tests show all syntax objectives are successfully understood by the parser, including all definition, statements and expressions. |
| Memory management objectives | Since their is no run time at all, the is no managed memory, thus the memory management objective has been met. |
| Command line interface objectives | All flags were used during development to debug the compiler, so all flags exist and the objectives have been met. |

### Feedback
> This project is most impressive. His extensive tests show the whole system has met the original objectives, and that the compiler is capable of some basic algorithms. The improvements for this kind of project are endless including:
> * Standard library
> * More syntax
> * Module system
> * Cross platform builds
> * Better distribution
> * REPL
> * etc

If I had more time I would have liked to add some of these features. A module system would present an interesting opportunity to parralize the compiler. For every included file an instance of the compiler would compile them to an AST, from their the AST's would be joined into a single tree which would be lowered into LLVM IR.

With a module system in place the implementation of a standard library would be trivial. When the compiler reaches an include/import statement it would first search the standard library before the working directory. All the standard library would be implemented in Fur, and eventually the compiler itself.

Cross platform builds and better distribution would involve porting some of the OS code to windows, perhaps even abstracting out the OS as an interface to the rest of the compiler. A dedicated website with automated builds would ensure anyone could get a copy of the compiler.