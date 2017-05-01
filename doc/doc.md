# Furlang: An investigation into modern programming languages and compilers

## Analysis

In this investigation the aim is to design a programming language and implement a compiler to create executable programs. Due to the time constraints it would be infeasible to implement all aspects of a modern programming language, standard library and language tooling. Instead the focus will be on implementing a sizeable subset such that simple algorithums like the greatest common devisor, bubble sort and fibbonanci sequence can be created.

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

### Overview
![](https://i.imgur.com/dA6xCKY.png)
1. Source code is parsed into the compiler
2. The lexer turns the source code into tokens, the smallest pieces of syntax
3. The parser turns the tokens into an abstract syntax tree
4. The AST is transformed into LLVM IR, which is a lower level language
5. LLVM uses lots of parses through the IR to optermize the code
6. LLVM produces assembly which is can then be linked by a linker, producing an executable

### Psudocode
This project was implemented in C which means their is alot of code which isnt important for an explanation on how each algoritum works. To help illustrate how the algorithums work I have opted to use psudocode so that it is easier to understand. The psudocode is close to the AQA psudocode spec with a few additions.

* `LABEL` and `ENDLABEL` make a labeled section of code
* `GOTO` goes to a label
* `_` is equivilent to `NULL` in C 

### String
Strings in C are represented by a pointer to some characters (that end with a null byte). This means that to append somthing to a string it would require a reallocation, which is slow. Additionaly to find the length of a C string, it requires a function call which loops until it reaches the null byte unlike most languages were it would be a constant time operation. It makes sence in this case to build a more dynamic string for when we dont know how long the string should be, and dont care about the additional memory.

Strings have are split into two parts, the header and the characters. So that strings are compatible with the C standard library all the pointers returned by the string functions point to the characters so the header is hidden and must be accessed through the string functions. The header contains the length of the string (for fast direct access) and the capacity of the string (how many character can be stored before a reallocation must occor).

#### String equality
One side effect of knowing the strings length without looping though the string is faster string equality checks. The C standard library compares two strings character by character, if their is a diffrence the function returns false (non-zero) however since we have constant time access to the string length we can first check if the two strings lengths are equal thus improving performance in the case where two strings are not the same length.

```
FUNCTION string_equals(string a, string b)
    length <- string_length(a)
    if length != string_length(b) THEN
        RETURN FALSE
    ELSE
        FOR i <- 0 TO length
            IF a[i] != b[i] THEN
                RETURN FALSE
            ENDIF
        LOOP
    ENDIF
    
    RETURN TRUE
ENDFUNCTION
```

#### string.h
This is the header file for the string implementation

```
#include "../src/include/string.h"
```

#### string.c
This is the source file for the string implmentation

```
#include "../src/string.c"
```

### Pool
A compiler often allocates and frees many of the same structures. You could use the standard memory allocation function `malloc()` and `free()` however these are slow for many small allocations and the data has bad locality, so more cache misses occor when the data is read. The solution to this problem is a custom allocator, in this case a pool allocator.

A pool allocator takes a large chunk of memory and divides it into equal parts. When ever a piece of memory is required, the pool allocator returns a section of this memory. When the memory is freed the pool allocator reclaims this memory ready to give out on the next request.

#### Constructing a pool
The pool must first allocate a large chunk of memory to form the pool. From their the free list is set up by looping through each slice of memory and adding a header with pointer to the next slice of memory. 

```
lastElement = memory[0]
FOR i = 0 TO elementCount
    element <- memory[i]
    lastElement.next <- element
    lastElement <- element
LOOP
lastElement.next <- _
```

Finaly we create a structure to save some details about the pool.
```
pool <- _
pool.memory = memory
pool.elementSize = elementSize
pool.elementCount = elementCount
pool.head = memory[0]
pool.tail = lastElement
RETURN pool
```

#### Pool full
If the pool is full then the free list is empty hence `pool.head` (and `pool.tail`) must be `NULL`

```
FUNCTION poolFull(pool) 
    return pool.head = _
ENDFUNCTION
```

#### Pool count
For debugging purposes its often usefull to know how much elements are in the pool. To compute this, we walk the free list to find out how much slices are free, then subtract that from `elementCount`

```
FUNCTION poolCount(pool)
    IF poolFull(pool) THEN
        return pool.elementCount
    ELSE
        free <- 0
        ptr <- pool.head
        WHILE ptr != pool.tail 
            free <- free + 1
            ptr <- ptr.next
        ENDWHILE
        free <- free + 1
        
        RETURN pool.elementCount - free
    ENDIF
ENDFUNCTION
```

#### Expaning the pool
Expading the pool is quite involved since the pools memory may have been moved by the reallocation. First we must save some details about the old pool.
```
oldCount <- pool.count
isFull <- poolFull(pool)
```
Then we compute the pointer offset of the head and tail so that we can restore them later.
```
oldMemory <- pool.memory
headOffset <- pool.head - oldMemory
tailOffset <- pool.tail - oldMemory
```
Now we can do the reallocation and restore the head and tail pointers.
```
pool.memory <- realloc(pool.memory, pool.elementSize * pool.elementCount)
pool.head <- pool.memory + headOffset
pool.tail <- pool.memory + tailOffset
```
The free list pointers are also invalidated by the reallocation so they too must be restored.
```
IF !isFull THEN
    ptr <- pool.head
    WHILE ptr != pool.tail
        ptr.next <- pool.memory + ptr.next - oldMemory
        ptr <- ptr.next
    ENDWHILE
ENDIF
```
Now that the pools capacity has expaned we need to expand the free list. First we set up a free list of all the new elements
```
firstNew <- pool.memory[oldCount]
lastElement <- firstNew
FOR i = oldCount TO pool.elementCount
    element <- pool.memory[i]
    lastElement.next = element
    lastElement = element
LOOP
```
The two lists can now be joined unless the pool was full before, then their was not old list so the new list becomes the free list.
```
IF isFull THEN
    pool.head <- firstNew
ELSE
    pool.tail.next <- firstNew
ENDIF
pool.tail <- lastElement
```
As you can see extending the pool is quite an expensive option, however the time it takes to (re)allocate a pool far outweighs the time taken to allocated many small structures.

#### Getting memory from the pool
If the pool is full then their is no memory to give, thus we expand the pool. Otherwise we return the next element in the free list, after removing it from the list.

```
FUNCTION poolGet(pool)
    IF poolFull(pool) THEN
        poolExpand(pool, pool.elementCount * 2)
    ENDIF
    
    element <- pool.head
    pool.head <- pool.head.next
    RETURN element
ENDFUNCTIO
```

#### Releasing memory back to the pool
Releasing memory back into the pool is as simple as adding the slice back into the free list. If the pool is full then the element starts a new freelist.
```
FUNCTION poolRelease(pool, element)
    IF poolFull(pool) THEN
        pool.head <- element
        pool.head.next <- _
        pool.tail <- pool.head
    ELSE
        pool.tail.next <- element
        pool.tail <- element
    ENDIF
ENDFUNCTION
```

#### pool.h
This is the header file for the pool implementation
```
#include "../src/includes/pool.h
```

#### pool.c
This is the source file for the pool implementation
```
#include "../src/pool.c
```

### Queue
Queues are an important are another important data structure which the C standard library doesnt provide, so I implemented my own. This queue uses a double ended linked list on the back end allowing for fast insertions and deletions from either end of the queue. This is not strickly a queue, and can also be used as a stack.

#### Queue push
To push onto a queue, the queue allocates a new element with a header which is returned to the caller for populating with data. To push to the frount we add the new element at the start of the queue and link the new element and old element.

```
FUNCTION queuePushFront(queue)
    queue.size <- queue.size + 1
    item <- malloc(queue.elementSize)
    
    item->prev <- _
    IF queue.first = _ THEN
        item.next <- _
        queue.last <- item
    ELSE
        item.next <- queue.first
        queue.first.prev <- item
    ENDIF
    queue.first <- item
    
    return item
ENDFUNCTION
```

Likewise to push to the back of the queue we use a simular routine, adding the new element at the end of the queue.

```
FUNCTION queuePushBack(queue)
    queue.size <- queue.size + 1
    item <- malloc(queue.elementSize)
    
    item->next <- _
    IF queue.first = _ THEN
        item.prev <- _
        queue.first <- item
    ELSE
        item.prev <- queue.last
        queue.last.next <- item
    ENDIF
    queue.last <- item
    
    return item
ENDFUNCTION
```

#### Queue pop
Popping from the queue is as simple as removing an item from the frount or back and updating the list.
```
FUNCTION queuePopFrount(queue)
    IF queue.first = _
        RETURN _
    ENDIF
    
    q.size <- q.size - 1
    item <- queue.first
    
    IF q.first = q.last THEN
        q.last <- _
    ENDIF
    
    q.first.prev <- _
    q.first <- q.first.next
    
    RETURN item
ENDFUNCTION
```

```
FUNCTION queuePopBack(queue)
    IF queue.last = _
        RETURN _
    ENDIF
    
    q.size <- q.size - 1
    item <- queue.last
    
    IF q.last = q.first THEN
        q.first <- _
    ENDIF
    
    q.last.next <- _
    q.last <- q.last.prev
    
    RETURN item
ENDFUNCTION
```

#### Queue destroy
Destroying the queue is as simple as walking along the linked list and freeing each node.

```
FUNCTION queueDestroy(queue)
    WHILE queue.first != _
        next <- queue.first.next
        free(queue.first)
        queue.first <- next
    ENDWHILE
ENDFUNCTION
```

#### queue.h
This is the header file for the queue implementation

```
#include "../src/includes/queue.h"
```

#### queue.c
This is the source file for the queue implementation

```
#include "../src/queue.c"
```

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
| OR | `\|` | Bitwise or symbol |
| OR_ASSIGN | `\|=` | Bitwise or assign symbol |
| LOR | `\|\|` | Logical or symbol |


#### Lexing
To convert the source to token the lexer runs through the file character by character invoking diffrent procedures depending on which character the head points to. For example when a `"` is at the read pointer we know its the start of a string literal which end when we reach the closing `"` character. In psudocode this looks like the following.

```
FUNCTION lex(readPointer) 
    tokens <- []
    
    WHILE readPointer != _ 
        SWITCH readPointer
            ...
            CASE '"':
                type <- STRING
                value <- ""
                next(readPointer)
                WHILE readPointer != `"`
                    value <- value + readPointer
                ENDWHILE
            ...
        ENDSWITCH
        
        tokens <- Token{type, value}
    ENDWHILE
ENDFUNCTION
```

#### Semicolon insertion
In C every statement ends in a semi-colon. This tells the compiler that it is the end of a statement, and that the next tokens form a seperate statement. Most code only has a single statement per line so almost all semi-colons are at the end of a line, thus if the compiler was to insert semicolons at the end of all lines we could emit semicolons from the source code as in langauges like Go and javascript. 

To insert semi-colons in the correct place a simple `semi` flag tells the compiler weather to insert a semi at the end of the line. If this flag is false `clearWhitespace` (which moves the `readPointer` past all the whitespace characters) skips over the newline without inserting a semi colon, else the `\n` character is detected in the lex routine and a semi colon is inserted in its place.

```
FUNCTION clearWhitespace()
    WHILE readPointer = ' ' OR 
        readpointer = '\t' OR 
        (readpointer = '\n' AND !semi) OR
        readpointer = '\r'
        
        next(readPointer)
        column -> column + 1
    ENDWHILE
ENDFUNCTION
```

#### Extracting a number
Fur has four types of numerical literals: ints, floats, octals and hexedecimals. Instead of a seperate procedure for each one which would require backtracking/look-head and alot of code duplication we use a single procedure. Note this uses gotos which are normaly a terrible idea, in this case they make the code more efficent, cleaner and easier to follow. 

The first part is `extractMantissa` which returns all the digits which are less than `base`. `isDigit` checks if a character literal is a digit (0-F) and `asDigit` converts a character literal to an integer. `character` is the current character the lexer is reading and `nextCharacter` moves the `character` pointer to the next character.  

```
FUNCTION extractMantissa(base)
  mantissa <- ""
  WHILE isDigit(character) && asDigit(character) < base THEN
    mantissa <- mantissa + character
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

#### lexer.h
This is the header file for the lexer implementation

```
#include "../src/includes/lexer.h"
```

#### lexer.c
This is the source file for the lexer implementation

```
#include "../src/lexer.c"
```

### Parser
The parser takes the list of tokens constructed by the lexer and transforms them into an abstract syntax tree, which is a tree structure which represents the program being compiled. An AST is constructed from nodes each with children for example the expression -a + 4 * 5 would be parsed into:
```go
BinaryNode {
	Left: UnaryNode {
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

Nodes are slit into 3 types, expressions, statements and declarations.

#### Expressions
Expression nodes are nodes that can be evaluated.

| Name | Example | Notes |
| ---- | ------- | ----- |
| identExp | `foo` | An identifier |
| literalExp | `123` | Any literal value including numbers and strings |
| unaryExp | `-100` | An expression whith a leading unary operation |
| binaryExp | `10 + 3` | Binary expression with an infix operation |
| selectorExp | `foo.bar` | A selection expression, for accessing keys in a struct |
| indexExp | `foo[0]` | An index expression, for accessing items in an array |
| callExp | `foo(bar, baz)` | Function call expression |
| keyValueExp | `foo: 123` | Expression with a (optional key) and value |
| keyValueListExp | `{foo: 123, bar: 321}` | List of key value expressions |
| structValueExp | `foo{bar: 123}` | Key value list with a procedding type |
| arrayExp | `[1, 2, 3]` | Array initilizer expression |
| arrayType | `int[10]` | Array type expression |

#### Statements
Statements provide the control flow and assignment syntax.

| Name | Example | Notes |
| ---- | ------- | ----- |
| declareSmt | `foo := 100` | Wrapper around declare node |
| assignmentSmt | `foo = 100` | Varible assignment |
| retSmt | `return 100` | Return statement |
| ifSmt | `if foo > 19 {}` | If statement |
| forSmt | `for i := 0; i < 10; i++ {}` | For statement |

#### Declarations
Declare nodes are the top level parent nodes.

| Name | Example | Notes |
| --- | --- | --- |
| argumentDcl | `int a` | Function argument declaration |
| functionDcl | `proc foo :: int bar, int baz -> int` | Function declaration |
| varibleDcl | `var int a = 100` | Varible declarations |

#### Pratt Parser

When it comes to language parsing their are many different options. A parser generator can automatically build a parser from a list of rules, however the error messages that they produce can be hard to understand and not very customizable. For this reason most languages opt to right their own parsers from scratch as I did with Fur. For this project I implemented a Top down operator precedence parser, also known as a Pratt parser, with a few modifications.

The key parts of this algorithm is the `led` and `nud` procedures. `led` (short for left denotation) parses the next token in an infix context i.e. the token joins two expressions. The original Pratt parser was only designed for expressions however I have added a a `stmd` function which parses a statement in a simular style.

##### Nud method
`nud` (short for null denotation) parser a token in a prefix context, i.e. the token starts a new expression.

```
FUNCTION nud(token)
	SWITCH typeof(token)
		# Case statements here ...
	END SWITCH
END FUNCTION
```
###### Literal expression
If a literal is at the beginning of an expression (or sub-expression) we return a literal node, any following operators will be handled in an infix context. This is a key distinction between some other algorithms, by delaying infix operators we are able to parse mathematical expressions like a + b * c without having to use something like the shunting yard algorithm which requires allocating a stack (two stacks for function calls) and queue.

```
CASE INT, FLOAT, HEX, OCTAL, STRING:
	return LiteralNode { token }
```

###### Ident expression
```
CASE IDENT:
	return IdentNode { token }
```

###### Unary expression
A unary node is normaly somthing like `-14`, the operator in front changes the value to the right. To parse a unary node we treat the tokens after the operator as a sub-expression by calling `expression` (see below). 

We parse in a value of 60 for the binding power of the sub expression, this is the other unique feature of a Pratt parser. To delay the parsing of infix tokens we use the consept of binding power, which is how well tokens should _bind_ to eachother. For example the binding power of `*` will be higher than the binding power of `-` since we alway multiply before subtracting. By calling `expression` we get the next subexpression where all the tokens are bound by a binding power higher than the right binding power (which is the parameter to expression).

`expression` is what gives the ast its tree structure and allows us to parse expressions such as `-(14 + a)`, in this case the expression on the right wasnt a simple literal value but a binary expression.

```
CASE NOT, SUB:
	return UnaryNode { token, expression(60) }
```

###### Key value expression
A key value node is a list of values with keys such as:
```
{
    baz: 123,
    qux: 2.4,
}
```
but the keys may be ommited 
```
{ 123, 2.4 }
```
To parse a list of values we use a loop to parse each key value pair until we reach a `}` which is the end of list. Inside the loop we try to parse a key or a value, since we dont know if the item has a key we must check if the next token is a `:`. If it is then the `keyOrValue` was a key and the value expression is parsed, if their was no colon then `keyOrValue` must be a value. A `KeyValue` node is then appended to the `keyValues` array which will form the `KeyValueList` node. 

```
CASE LPAREN:
    count <- 0
    keyValues <- []
    
    WHILE token != RBRACE DO
        keyOrValue <- expression()
        key <- NULL
        value <- NULL

        if token == COLON THEN
            nextToken()
            key <- keyOrValue
            value = expression()
        ELSE
            value = keyOrValue
        ENDIF
        
        keyValues[count] <- KeyValue {key, value}
        count <- count + 1
        
        expect(COMMA)
    ENDWHILE
    
    RETURN KeyValueList{keyValues}
```

###### Array expression
Array nodes are very simular to a `KeyValueList` expression however they can ommit the check for keys since array initilizers dont have keys

```
CASE LPAREN:
    count <- 0
    values <- []
    
    WHILE token != RBRACK DO
        value <- expression()
        values[count] <- value
        count <- count + 1
        expect(COMMA)
    ENDWHILE
    
    RETURN Array{keyValues}
```

##### Led method
The led method parses a token that in an infix context, i.e. between two nodes.
```
FUNCTION led(exp, token)
    rbp <- getBindingPower(token)
    SWITCH typeof(token)
        # Case statements here ...
    END SWITCH
END FUNCTION
```

###### Binary expression
A binary expression is like a unary expression except the operator is in the middle of two values such as `41 + 2`. In this case `41` would have already been parsed and `+` is the current token. To finish the node the right hand side must be constructed by a call to `expression`.

Assignment statements are also parsed here and then transformed into the correct node elsewhere otherwise some look ahead functionality would be required which would make the parser less efficent.

```
CASE ADD, SUB, MUL, QUO, REM, 
    EQL, NEQ, GTR, LSS, GEQ, LEQ, LAND, LOR,
    ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
    
    RETURN BinaryExp{exp, token, expression(rbp)}
```

###### Selector expression
A selector expression is exactly the same as a binary expression except the two values are seperated by a `.`.

```
CASE PERIOD:
    RETURN SelectorExp{exp, expression(rbp)}
```

###### Call expression
A call expression is simular to a key value list expression.

```
CASE LPAREN:
    count <- 0
    parameters <- []
    
    WHILE token != RPAREN DO
        param <- expression()
        parameters[count] <- param
        count <- count + 1
        expect(COMMA)
    ENDWHILE
    
    RETURN Call{parameters, exp}
```

##### Smtd method
Statment parses the token in the context of the start of a _statement_, instead of a expression as in `nud`. This is an important distinction since the same token can mean diffrent things if it is at the start of an expression of statement.

###### Return statement
Return statements are simple nodes with a single expression being the value the statement returns.
```
CASE RETURN:
    RETURN Return{expression()}
```

###### Block statement
Block statements are used in more complex statements such as if and for statments or on their own.
```
CASE LBRACE:
    expect(LBRACE)
    smtCount <- 0
    smts <- []
    WHILE token != RBRACE THEN
        smtCount <- smtCount + 1
        smts[smtCount] <- statement()
        expect(SEMI)
    ENDWHILE
    expect(RBRACE)
    RETURN Block{smts}
```

###### If statement
To parse an if statements we look for the `IF` token, then parse the condition as an expression and the block as a block. If their is an `ELSE` after the block, we check for an `IF`. If their is an `IF` we recursivly call `if()` to parse the next if, otherwise it must be a final else with no condition.

```
CASE IF:
    expect(IF)
    condition <- expression()
    block <- block()
    else <- _
    IF token == ELSE THEN
        expect(ELSE)
        IF token == IF THEN
            else <- if()
        ELSE
            else <- If{_, block(), _}
        ENDIF
    ENDIF
    
    RETURN If{condition, block, else}
```

###### For statement
For statments are simular to if statements

```
CASE FOR:
    expect(FOR)
    index <- declaration()
    condition <- expression()
    body <- block()
```

###### Variable declaration
Varible declarations are normaly handled elsewhere however if it starts with the optional `var` then we can parse the varible declaration now (rather than later).

```
CASE VAR:
    RETURN declaration()
```

###### Increment expression
Increment statements dont start with a keyword, so we look for an identifier. If their is not a proceeding increment or decrement token then we know the expression is a assignment or declaration node so we let the caller handle it.

```
CASE IDENT:
    ident <- Ident()
    SWITCH token
        CASE INC:
            RETURN binary{ident, ADD_ASSIGN, 1}
        CASE DEC:
            RETURN binary{ident, SUB_ASSIGN, 1}
    ENDSWITCH
    RETURN _
```

##### Declarations
Their are only two declaration nodes, functions and varibles.

###### Function declaration
Function declaration are more complicated than most nodes since they contain alot of infomation

```
CASE PROC:
    expect(PROC)
    name <- Ident()
    expect(DOUBLE_COLON)
    
    args <- []
    argCount <- 0
    WHILE token != ARROW OR token != LBRACE
        type <- Type()
        name <- Ident()
        args[argCount] = Arg{type, name}
        argCount <- argCount + 1
    ENDWHILE
    
    expect(ARROW)
    
    returnType <- Type()    
    
    body <- Block()
    
    RETURN Function{name, args, returnType, body}
```

###### Varible declaration
Variable declaration come in two forms, short form such as `foo := 123` and `var bar int = 199`. In the case of the long form declaration we know ahead of time that is a varible declaration since it starts with the var keyword.

```
CASE VAR:
    type <- Type()
    name <- Ident()
    value <- Expression()
    RETURN VariableDcl{type, name, value}
```

Otherwise the statement wrapper function first trys to parse it as a statement with `smtd()`, for short varible declarations this will return `NULL`. Next it trys parsing it as an expression, this will return a binary expression node where the operator is a assign or define type. From their the expression is converted into a statement.

```
FUNCTION Statement()
    statement <- smtd()
    IF statement != _ THEN
        RETURN statement
    ENDIF
    
    expression <- Expression()
    IF expression = _ THEN
        RETURN _
    ENDIF
    
    SWITCH expression.op
        CASE ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
            RETURN Assignment{expression.left, expression.op, expression.right}
        CASE DEFINE:
            RETURN VariableDcl{expression.left, expression.right}
    ENDSWITCH
ENDFUNCTION
```

##### Error handling
Error handling inside a parser is particulaly difficult, if their an unexpect token anywhere, we cant parse the current expression, statement or declaration however their may be other errors further down. Most compilers will no halt at a single error, but continue so that any other errors can be reported in a single compilation which makes finding and fixing bugs much easier.

So whenever we call `expect()` we return the token we were expecting or `NULL` if the token was not what we expected. If the token was incorrect an error is produced and added to the array of errors. It is up to the caller how to handle and incorrect token, generaly their is three diffrent options.

* If the token is non fatel such as a `::` in a function definition, the parser can continue from were it is.
* If we are inside a statement and the token was fatel we can skip over the tokens until a `SEMI` token is reached, then the parser can continue safely.
* Otherwise the whole block must be skipped.

If the parser doesnt skip over the affected tokens, the same line of source code would have many confilicting errors which wouldnt make sence.

```
FUNCTION skipBlock()
    WHILE token != LBRACE
        nextToken()
    ENDWHILE
    
    depth <- 0
    REPEAT
        IF token = LBRACE THEN
            depth <- depth + 1
        ELSEIF token = RBRACE THEN
            depth <- depth - 1
        ENDIF
        nextToken()
    WHILE depth > 0
ENDFUNCTION
```

```
FUNCTION skipSemi()
    WHILE token != SEMI THEN
        nextToken()
    ENDWHILE
    nextToken()
ENDFUNCTION
```

#### parser.h
This is the header file for the parser implementation
```
#include "../src/includes/parser.h"
```

#### parser.c
This is the source file for the parser implementation
```
#include "../src/includes/parser.c"
```

### IR Generation
Having converted the source code to tokens and the tokens to an abstract syntax tree we now can transform it into LLVM IR which LLVM will compile to machine code. LLVM IR is much lower level than Fur however the LLVM C API does alot of the heavy lifting for by constructuing the intemediantry language in memory.

#### If statements
LLVM IR does not have any if statements only conditional branches. To brach we must break the code into seperate blocks which end with a terminating statements (such as a branch, return etc). Since if statements can be chained it make sence for the transformation to be recursive hence we need a two functions, the recursive `compileIfBranch()` function and the `compileIf()` function to call into the recursive function.

`compileIf()` just sets up a `endBlock` which is the block which the program will continue executing from.
```
FUNCTION compileIf(irgen, ifSmt)
    endBlock <- LLVMAppendBasicBlock(irgen.function, "endBlock")
    compileIfBranch(irgen, ifSmt, _, endBlock)
ENDFUNCTION
```

`compileIfBranch()` takes four arguments: a refrence to irgen which holds some state about the ir generation, the if node, the block to branch from (will be `NULL` for root level if branches) and the block to continue execution from. 

The first step is to check for a condition, if the condition is `NULL` then this branch must be an else branch therefore we compile the block and exit. `CompileBlockAt()` compiles a block statement and returns an `LLVMBlockRef` to were the execution should continue. We use `SetBlock()` to set it as are current block and check if its not terminated, if its not then we insert a branch to the endblock.
```
condition <- ifSmt.condition
IF condition = _ THEN
    outBlock <- CompileBlockAt(irgen, ifSmt.body, block)
    
    SetBlock(irgen, outBlock)
    IF LLVMGetBasicBlockTerminator(outBlock) = _ THEN
        LLVMBuildBr(irgen.builder, endBlock)
        SetBlock(irgen, endBlock)
    ENDIF
    
    RETURN
ENDIF
```

From this point we know the if branch must be an if or an else if. The next step is to get the `parent` block, which is the block to branch from.
```
parent <- _
IF block = _ THEN
    parent <- irgen.block
ELSE
    parent <- block
ENDIF
```

`block` now becomes the block if the condition is true
```
block <- LLVMAppendBasicBlock(irgen.function, "if")
```

`falseBlock` will be either the next else/else if branch or the block to continue execution
```
falseBlock <- _
IF ifSmt.else != _ THEN
    falseBlock <- LLVMAppendBasicBlock(irgen.function, "else")
ELSE
    falseBlock <- endBlock
ENDIF
```

Now we can compile the body of the current if/else if branch, if the execution is not terminated we also branch to `endBlock`
```
outBlock <- CompileBlockAt(irgen, ifSmt.body, block)
IF LLVMGetBasicBlockTerminator(outBlock) = _ THEN
    SetBlock(irgen, outBlock)
    LLVMBuildBr(irgen.builder, endBlock)
    SetBlock(irgen, parent)
ENDIF
```

The conditional branch instruction is next.
```
condition <- CompileExp(irgen, cond);
LLVMBuildCondBr(irgen.builder, condition, block, falseBlock);
```

Next we check for any chaining else if/else blocks. If their are we recursivly call this function to compile the whole chain.
```
SetBlock(irgen, falseBlock)
IF ifSmt.else != _ THEN
    CompileIfBranch(irgen, ifSmt.else, falseBlock, endBlock)
ENDIF
```

Finaly we set the endBlock as the current block so any further instructions execute from their.
```
SetBlock(irgen, endBlock)
```

#### irgen.h
This is the header file for the IR generation implementation
```
#include "../src/includes/irgen.h"
```

#### irgen.c
This is the source file for the IR generation implementation
```
#include "../src/includes/irgen.c"
```

## Technical solution

### build/
This folder is were the make file produces the executables and librarys for the project.

#### CMakeLists.txt
This is the make file for the project
```
#include "../build/CMakeLists.txt
```

### src/
This is the folder were all the source files for the compiler reside.

#### includes/all.h
Contains a few helpfull defines for the whole project.
```
#include "../src/includes/all.h"
```

#### includes/ast.h
Typedefs for all the ast nodes.
```
#include "../src/includes/ast.h`
```

#### ast.c
Constructors for the AST nodes.
```
#include "../src/ast.c"
```

#### includes/error.h
```
#include "../src/includes/error.h"
```

#### error.c
Pretty prints errors to the terminal.
```
#include "../src/error.c"
```

#### includes/irgen.h
```
#include "../src/includes/irgen.c"
```

#### irgen.c
```
#include "../src/irgen.c"
```

#### includes/lexer.h
```
#include "../src/includes/lexer.h"
```

#### lexer.c
```
#include "../src/lexer.c"
```

#### lib.c
Includes all the source files so the compiler can be exposed as a library (which we use in the unit tests).
```
#include "../src/lib.c
```

#### main.c
Entry point for the compiler
```
#include "../src/main.c"
```

#### includes/parser.h
```
#include "../src/includes/parser.h"
```

#### parser.c
```
#include "../src/parser.c"
```

#### includes/pool.h
```
#include "../src/includes/pool.h"
```

#### pool.c
```
#include "../src/pool.c"
```

#### includes/queue.h
```
#include "../src/includes/queue.h"
```

#### queue.c
```
#include "../src/queue.c"
```

#### includes/string.h
```
#include "../src/includes/string.h"
```

#### string.c
```
#include "../src/string.c"
```

#### includes/uthash.h
The only other external dependencie (apart from LLVM), uthash which is a single header hash table.
```
#include "../src/includes/uthash.h"
```

### tests/
This folder contains the C++ program that tests the compiler

#### tests/add.fur
```
#include "../tests/tests/add.fur
```

#### tests/arrayInit.fur
```
#include "../tests/tests/arrayInit.fur
```

#### tests/arraySum.fur
```
#include "../tests/tests/arraySum.fur
```

#### tests/binaryFloat.fur
```
#include "../tests/tests/binaryFloat.fur
```

#### tests/binaryInt.fur
```
#include "../tests/tests/binaryInt.fur
```

#### tests/bubbleSort.fur
```
#include "../tests/tests/bubbleSort.fur
```

#### tests/fibbonanci.fur
```
#include "../tests/tests/fibbonanci.fur
```

#### tests/for.fur
```
#include "../tests/tests/for.fur
```

#### tests/gcd.fur
```
#include "../tests/tests/gcd.fur
```

#### tests/if.fur
```
#include "../tests/tests/if.fur
```

#### tests/ifElse.fur
```
#include "../tests/tests/ifElse.fur
```

#### tests/ifElseIfElse.fur
```
#include "../tests/tests/ifElseIfElse.fur
```

#### tests/ifElseIfElseIfElse.fur
```
#include "../tests/tests/ifElseIfElseIfElse.fur
```

#### tests/literal.fur
```
#include "../tests/tests/literal.fur
```

#### tests/longVar.fur
```
#include "../tests/tests/longVar.fur
```

#### tests/nestedFor.fur
```
#include "../tests/tests/nestedFor.fur
```

#### tests/reassignArg.fur
```
#include "../tests/tests/reassignArg.fur
```

#### tests/shortVar.fur
```
#include "../tests/tests/shortVar.fur
```

#### tests/unary.fur
```
#include "../tests/tests/unary.fur
```

#### irgen_test.cpp
Contains unit tests for the ir generation and the integrations tests for the whole system
```
#include "../tests/irgen_test.cpp"
```

#### lexer_test.cpp
Unit tests for the lexer
```
#include "../tests/lexer_test.cpp"
```

#### parser_test.cpp
Unit tests for the parser
```
#include "../tests/parser_test.cpp"
```

#### pool_test.cpp
Unit tests for the pool
```
#include "../tests/pool_test.cpp"
```

#### queue_test.cpp
Unit tests for the queue
```
#include "../tests/queue_test.cpp"
```

#### string_test.cpp
Unit tests for the string
```
#include "../tests/string_test.cpp"
```

#### test.cpp
Test program entry point
```
#include "../tests/test.cpp
```

## Testing
When creating this project I used the test driven development workflow in which you write a test first, watch it fail, then write the code to make it parse. By following this I have constructed a test program which tests every unit of the compiler as well as intergration tests to test the whole system. This is so important when building software to ensure no new code breaks any features.

## Evaluation
In the analysis I stated that "simple algorithums like the greatest common devisor, bubble sort and fibbonanci sequence" should be able to be made in Fur. Each of these algorithums are included as part of the integrations tests, which all pass, so I would say the final program meets the requirements.

## References
1. The Rust Reference <a id="1">https://doc.rust-lang.org/reference.html</a>
2. The Go Programming Language Specification <a id="2">https://golang.org/ref/spec</a>
3. Specification for the D Programming Language <a id="3">https://dlang.org/spec/spec.html</a>
4. The F# Language Specification <a id="4">http://fsharp.org/specs/language-spec/</a>
5. Go's GitHub Repository <a id="5">https://github.com/golang/go</a>
6. Go - Proposal: Eliminate STW stack re-scanning <a id="6">https://golang.org/design/17503-eliminate-rescan</a>
7. Plumber - G1 vs CMS vs Parallel GC <a id="7">https://plumbr.eu/blog/garbage-collection/g1-vs-cms-vs-parallel-gc</a>
