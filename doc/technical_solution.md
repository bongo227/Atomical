## Technical solution

### build/
This folder is were the make file produces the executable and library for the project.

#### CMakeLists.txt
This is the make file for the project
```cmake
#import "../build/CMakeLists.txt"
```

### src/
This is the folder were all the source files for the compiler reside.

#### includes/all.h
Contains a few helpfull defines for the whole project.
```c
#import "../src/includes/all.h"
```

#### includes/ast.h
Typedefs for all the ast nodes.
```c
#import "../src/includes/ast.h"
```

#### ast.c
Constructors for the AST nodes.
```c
#import "../src/ast.c"
```

#### includes/error.h
```c
#import "../src/includes/error.h"
```

#### error.c
Pretty prints errors to the terminal.
```c
#import "../src/error.c"
```

#### includes/irgen.h
```c
#import "../src/includes/irgen.h"
```

#### irgen.c
```c
#import "../src/irgen.c"
```

#### includes/lexer.h
```c
#import "../src/includes/lexer.h"
```

#### lexer.c
```c
#import "../src/lexer.c"
```

#### lib.c
Includes all the source files so the compiler can be exposed as a library (which we use in the unit tests).
```c
#import "../src/lib.c"
```

#### main.c
Entry point for the compiler
```c
#import "../src/main.c"
```

#### includes/parser.h
```c
#import "../src/includes/parser.h"
```

#### parser.c
```c
#import "../src/parser.c"
```

#### includes/pool.h
```c
#import "../src/includes/pool.h"
```

#### pool.c
```c
#import "../src/pool.c"
```

#### includes/queue.h
```c
#import "../src/includes/queue.h"
```

#### queue.c
```c
#import "../src/queue.c"
```

#### includes/string.h
```c
#import "../src/includes/string.h"
```

#### string.c
```c
#import "../src/string.c"
```

#### includes/uthash.h
The only other external dependency (apart from LLVM), uthash which is a single header hash table.
```c
#import "../src/includes/uthash.h"
```

### tests/
This folder contains the C++ program that tests the compiler

#### tests/add.fur
```markdown
#import "../tests/tests/add.fur"
```

#### tests/arrayInit.fur
```markdown
#import "../tests/tests/arrayInit.fur"
```

#### tests/arraySum.fur
```markdown
#import "../tests/tests/arraySum.fur"
```

#### tests/binaryFloat.fur
```markdown
#import "../tests/tests/binaryFloat.fur"
```

#### tests/binaryInt.fur
```markdown
#import "../tests/tests/binaryInt.fur"
```

#### tests/bubbleSort.fur
```markdown
#import "../tests/tests/bubblesort.fur"
```

#### tests/fibbonanci.fur
```markdown
#import "../tests/tests/fibbonanci.fur"
```

#### tests/for.fur
```markdown
#import "../tests/tests/for.fur"
```

#### tests/gcd.fur
```markdown
#import "../tests/tests/gcd.fur"
```

#### tests/if.fur
```markdown
#import "../tests/tests/if.fur"
```

#### tests/ifElse.fur
```markdown
#import "../tests/tests/ifElse.fur"
```

#### tests/ifElseIfElse.fur
```markdown
#import "../tests/tests/ifElseIfElse.fur"
```

#### tests/ifElseIfElseIfElse.fur
```markdown
#import "../tests/tests/ifElseIfElseIfElse.fur"
```

#### tests/literal.fur
```markdown
#import "../tests/tests/literal.fur"
```

#### tests/longVar.fur
```markdown
#import "../tests/tests/longVar.fur"
```

#### tests/nestedFor.fur
```markdown
#import "../tests/tests/nestedFor.fur"
```

#### tests/reassignArg.fur
```markdown
#import "../tests/tests/reassignArg.fur"
```

#### tests/shortVar.fur
```markdown
#import "../tests/tests/shortVar.fur"
```

#### tests/unary.fur
```markdown
#import "../tests/tests/unary.fur"
```

#### irgen_test.cpp
Contains unit tests for the IR generation and the integrations tests for the whole system
```cpp
#import "../tests/irgen_test.cpp"
```

#### lexer_test.cpp
Unit tests for the lexer
```cpp
#import "../tests/lexer_test.cpp"
```

#### parser_test.cpp
Unit tests for the parser
```cpp
#import "../tests/parser_test.cpp"
```

#### pool_test.cpp
Unit tests for the pool
```cpp
#import "../tests/pool_test.cpp"
```

#### queue_test.cpp
Unit tests for the queue
```cpp
#import "../tests/queue_test.cpp"
```

#### string_test.cpp
Unit tests for the string
```cpp
#import "../tests/string_test.cpp"
```

#### test.cpp
Test program entry point
```cpp
#import "../tests/test.cpp"
```