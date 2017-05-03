# Furlang: An investigation into modern programming languages and compilers

## Analysis

In this investigation the aim is to design a programming language and implement a compiler to create executable programs. Due to the time constraints it would be infeasible to implement all aspects of a modern programming language, standard library and language tooling. Instead the focus will be on implementing a sizeable subset such that simple algorithms like the greatest common divisor, bubble sort and Fibonacci sequence can be created.

### Background
The first recognizable modern computers were created in the 1940's. Their limited hardware meant programmers would write hand tuned assembly which had none of the abstraction of modern languages meaning programs were slow to develop and error-prone. Autocode, developed in the 1950's, was the first higher-level compiled language. The invention of a compiler meant programmers could write less code and rely on the compiler to make optermizations that had previously required a large amount of knowledge to implement.

Fortran was invented in 1954 at IBM, it was the first higher level functional language. To this day fortran can be seen in high performace code bases espesically in the acedemic areas of computer science like machine learning. List is another language developed towards the enfo of the 1950's that is still in use today. Lisp brougt lexical scoping which allowed a block to hide its varibles and procedures from code outside that block.

The 1960's - 70's saw the invention of much of the syntax and paradigms used today. Simula was the first langauge to support object oriented programing were code is seperated into many objects which have methods and attributes much like real objects. ML brought the first staically typed functional programming languse with its polymorphic type system. C was developed at Bell labs and quickly became one of the most important languages ever, powering many low level systems to this day.

The 80's broght alot of refinement such as the idea of organizing code into modules which helped make large code bases more maintainable and generic programming which meant programmers could write functions that worked for many diffrent types rather than many functions for each possible type. Computer instrubtion sets got smaller and more efficent which lead way to many improvements in the speed of programs which meant more advance systems could be designed.

The explosive growth of the internet in the 90's caused widespread development and use of scripting languages, especially JavaScript which powers almost all of the interactive portions of websites to this day. Other developments included the integrated development environment which packaged all the languages tool into a text editor which made programming easier and faster. Garbage collection meant that memory no longer needed to be managed by the programmer, instead the run time would detect when data was not being used and recycle for when the programmer needed new memory.

Since then concurrent constructs such as callbacks and language level schedulers for light weight threads meant that programs could more easily take advantage of a modern processor with many cores. The idea of open source where programmers would post the code online so anyone could use and improve the software. OpenCL and CUDA allowed programmers to write massively parallel software for the GPU.

### Research
In terms of the languages design I looked at several languages with similar goals as mine and read through their specifications including: [Rust<sup>[1]</sup>](#1), [Go<sup>[2]</sup>](#2), [D<sup>[3]</sup>](#3) and [F#<sup>[4]</sup>](#4). I looked at their syntax and the design decision behind them in order the judge what code should look like.

### Structure
Most compilers are three phase compilers which split the compilation into three parts: frount end, middle end and back end. The front end verifies the syntax and semantics, providing warnings if their are any issues. The front end is produces intermediate representation which is a lower level language more easily manipulated by the other phases than the higher level source language. This is the part of the compiler I will be creating, LLVM will be used for the middle and back end.

The middle end preforms platform indepented optermizations such as dead code elimination, loop unrolling, constant propagation etc. LLVM does this by passing through the source code many times, each time performing a particular optermization.

The back end performs target specific analysis and optermizations before producing the machine code for the target machine.

### Syntax
Compared to C modern programming languages use a lot less characters to describe the instructions which make the final program. By using less character it becomes a lot faster to read through the source code in order to understand the logic, which intern makes the language easier to use and faster to develop in. With Fur, I wanted to explore some of these modern ideas and how they can be implemented. 

#### Type inference
In go, most variables do not need a type because their type can be inferred:
```go
foo := 123
```
In this case foo will be an `int` type since the value on the right hand side is an integer. If you want to be more explicit you can use the longer form variable declaration:
```go
var foo int = 123
``` 
The inferred type is much quicker to type and just as easy to read, helping to reduce the character count of the source code. 

#### Semicolons and parenthesis
Most languages use semi colons to tell the compiler that the current statement has ended and everything after the semicolon is interpreted as a separate statement. In more modern languages like python, their are no semicolons anywhere in the language. Instead python uses spaces and new lines to signify the start and end of each statement and block.

Go uses semicolons only in a few places such as for loops:
```go
for i := 0; i < 10; i++ {}
```
Here each part of the for loop is separate by semicolons just like C, however for most statements new lines are used as in python to signify the end of a statement.

Another thing to note is the lack of parenthesis around the for loop. The lack of brackets helps to further eliminate useless syntax which only hinders the programmer. The side effect of committing semicolons and brackets is that the source code is much more complex to parse since the compiler must insert semicolons before the syntax can be parsed (or use some other method to deal with the lack of semicolons).

#### Function definitions
I decided to create my own syntax for function definitions inspired partly by functional programming languages. 
```
proc bar :: int foo -> float
```
The double semi colon is used to provide a clear divider between the name and the arguments, this clear line of separation helps when skimming though the source code in order to find a function with a certain name. Also the arrow that separates the arguments and return type reinforces the concept of a function, to take input and transform it into output.

#### Keywords
Golang has [25 keywords<sup>[2]</sup>](#2) which helps make it's easy/quick to learn, simple to read and less keywords are reserved so it's simpler to name some constructs. The obvious drawback is the reduced expressiveness compared to languages like C# and Java which have many more keywords. Fur will be closer to Go in terms of the small pool of keywords in favour of a more simpler and clearer expression of logic.

#### Symbols
Functional languages like F# tend to use far more symbols to express their logic. Symbols make smaller code and less visual noise however they create a steeper learning curve since they are less descriptive and often unique to each language. Fur should feel familiar, sharing most of the symbols from C based languages.

#### Operator overloading
C++ and Java both have operator overloading which makes their source code easy to read in code bases that use things like arbitrarily wide integers, vectors and matrices with complex mathematical operations. The problem is operator overloading is easily abused by hiding complex computations behind a simple looking operator. For example in the case of a two arbitrary length integers being multiplied, memory allocations and complex loops are invoked which isn't obvious from the call site. To help to programmer reason about the performance and create fast efficient code fur will opt for no function overloading.

#### Syntax objectives
* It must parse the definition syntax
  * Functions definitions such as `proc add :: int a, int b -> int`
* It must parse the statement syntax
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
  * Block statements such as `{ foo := 100 }`
  * If statements such as `if foo > 100 {}`
  * For statements such as `for i := 0; i < 100; i++ {}`
* It must parse the expression syntax
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

One solution to this problem is a garbage collector. In languages such as Go and Java the run-time allocates blocks of memory, whenever the program requests memory a portion of that block is returned. The run-time then keeps track of where the memory is used, when the portion of memory is no longer in use its is marked and recycled ready for the next allocation. Over time these garbage collectors have been getting faster and much more sophisticated, one consequence of this is that it can be hard to understand their behaviour.

The problems arises in applications which have low latency requirements, such as games. With virtual reality and higher refresh rate monitors, games have less than 7 milliseconds to update and render the next frame. GC max pause times in Go are around [50µs<sup>[6]</sup>](#6) (with considerable CPU usage) and [50ms<sup>[7]</sup>](#7) in Java, what's worse is that they can happen at anytime causing the game to freeze and stutter. One workaround is to budget this into your frame time i.e. 5ms to update and render and 2ms for any GC pauses, this means reduced graphics, less realistic physics and simpler game mechanics. Even if you do not pause mid-frame there is still the problem of: higher read/write latency, less CPU performance and less data locality (hence less cache utilisation). For these reason Fur will not have a garbage collector.

#### Memory management objectives
 * Compile programs should have no run time managed memory.

### Command line interface
Compilers normally expose a command line interface to transform the syntax into an executable file. This makes it very easy to integrate the compiler with other build tools, text editors and IDE's. Fur doesn't require a large range of flags and options since it is a small language however some debug options could help the programmer (and the development of the compiler).

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
This project was implemented in C which means their is a lot of code which is not important for an explanation on how each algorithm works. To help illustrate how the algorithms work I have opted to use psudocode so that it is easier to understand. The psudocode is close to the AQA psudocode spec with a few additions.

* `LABEL` and `ENDLABEL` make a labeled section of code
* `GOTO` goes to a label
* `_` is equivilent to `NULL` in C 

### String
Strings in C are represented by a pointer to some characters (that end with a null byte). This means that to append something to a string it would require a reallocation, which is slow. Additionally to find the length of a C string, it requires a function call which loops until it reaches the null byte unlike most languages were it would be a constant time operation. It makes sense in this case to build a more dynamic string for when we do not know how long the string should be, and do not care about the additional memory.

Strings have are split into two parts, the header and the characters. So that strings are compatible with the C standard library all the pointers returned by the string functions point to the characters so the header is hidden and must be accessed through the string functions. The header contains the length of the string (for fast direct access) and the capacity of the string (how many character can be stored before a reallocation must occur).

#### String equality
One side effect of knowing the strings length without looping though the string is faster string equality checks. The C standard library compares two strings character by character, if their is a difference the function returns false (non-zero) however since we have constant time access to the string length we can first check if the two strings lengths are equal thus improving performance in the case where two strings are not the same length.

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
#import "../src/includes/string.h"
```

#### string.c
This is the source file for the string implementation

```
#import "../src/string.c"
```

### Pool
A compiler often allocates and frees many of the same structures. You could use the standard memory allocation function `malloc()` and `free()` however these are slow for many small allocations and the data has bad locality, so more cache misses occur when the data is read. The solution to this problem is a custom allocator, in this case a pool allocator.

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

Finally we create a structure to save some details about the pool.
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
For debugging purposes its often useful to know how much elements are in the pool. To compute this, we walk the free list to find out how much slices are free, then subtract that from `elementCount`

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

#### Expanding the pool
Expanding the pool is quite involved since the pools memory may have been moved by the reallocation. First we must save some details about the old pool.
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
Releasing memory back into the pool is as simple as adding the slice back into the free list. If the pool is full then the element starts a new free list.
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
#import "../src/includes/pool.h"
```

#### pool.c
This is the source file for the pool implementation
```
#import "../src/pool.c"
```

### Queue
Queues are an important are another important data structure which the C standard library doesn't provide, so I implemented my own. This queue uses a double ended linked list on the back end allowing for fast insertions and deletions from either end of the queue. This is not strictly a queue, and can also be used as a stack.

#### Queue push
To push onto a queue, the queue allocates a new element with a header which is returned to the caller for populating with data. To push to the front we add the new element at the start of the queue and link the new element and old element.

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
Popping from the queue is as simple as removing an item from the front or back and updating the list.
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
#import "../src/includes/queue.h"
```

#### queue.c
This is the source file for the queue implementation

```
#import "../src/queue.c"
```

### Lexer
The lexer's job is to turn the source code into a sequence of tokens. A token is the smallest possible bit of meaningful source code such as a number or name. The following is all the possible types of token:

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
To convert the source to token the lexer runs through the file character by character invoking different procedures depending on which character the head points to. For example when a `"` is at the read pointer we know its the start of a string literal which end when we reach the closing `"` character. In psudocode this looks like the following.

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
In C every statement ends in a semi-colon. This tells the compiler that it is the end of a statement, and that the next tokens form a separate statement. Most code only has a single statement per line so almost all semi-colons are at the end of a line, thus if the compiler was to insert semicolons at the end of all lines we could emit semicolons from the source code as in languages like Go and JavaScript. 

To insert semi-colons in the correct place a simple `semi` flag tells the compiler weather to insert a semi at the end of the line. If this flag is false `clearWhitespace` (which moves the `readPointer` past all the white space characters) skips over the newline without inserting a semi colon, else the `\n` character is detected in the lex routine and a semi colon is inserted in its place.

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
Fur has four types of numerical literals: int's, floats, octal's and hexadecimals. Instead of a separate procedure for each one which would require backtracking/look-head and a lot of code duplication we use a single procedure. Note this uses goto's which are normally a terrible idea, in this case they make the code more efficient, cleaner and easier to follow. 

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

This is the main part of the algorithm. If first checks for hexadecimal's or octal's then integers and floats, when we check for octal's we may find that it was actually an integer (or float) in which case we can jump to the appropriate point. 
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
#import "../src/includes/lexer.h"
```

#### lexer.c
This is the source file for the lexer implementation

```
#import "../src/lexer.c"
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

When it comes to language parsing their are many different options. A parser generator can automatically build a parser from a list of rules, however the error messages that they produce can be hard to understand and not very customizable. For this reason most languages opt to right their own parser from scratch as I did with Fur. For this project I implemented a Top down operator precedence parser, also known as a Pratt parser, with a few modifications.

The key parts of this algorithm is the `led` and `nud` procedures. `led` (short for left denotation) parses the next token in an infix context i.e. the token joins two expressions. The original Pratt parser was only designed for expressions however I have added a a `stmd` function which parses a statement in a similar style.

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
A unary node is normally something like `-14`, the operator in front changes the value to the right. To parse a unary node we treat the tokens after the operator as a sub-expression by calling `expression` (see below). 

We parse in a value of 60 for the binding power of the sub expression, this is the other unique feature of a Pratt parser. To delay the parsing of infix tokens we use the concept of binding power, which is how well tokens should _bind_ to each other. For example the binding power of `*` will be higher than the binding power of `-` since we always multiply before subtracting. By calling `expression` we get the next sub expression where all the tokens are bound by a binding power higher than the right binding power (which is the parameter to expression).

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
To parse a list of values we use a loop to parse each key value pair until we reach a `}` which is the end of list. Inside the loop we try to parse a key or a value, since we do not know if the item has a key we must check if the next token is a `:`. If it is then the `keyOrValue` was a key and the value expression is parsed, if their was no colon then `keyOrValue` must be a value. A `KeyValue` node is then appended to the `keyValues` array which will form the `KeyValueList` node. 

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
Array nodes are very similar to a `KeyValueList` expression however they can omit the check for keys since array initialiser does not have keys

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

Assignment statements are also parsed here and then transformed into the correct node elsewhere otherwise some look ahead functionality would be required which would make the parser less efficient.

```
CASE ADD, SUB, MUL, QUO, REM, 
    EQL, NEQ, GTR, LSS, GEQ, LEQ, LAND, LOR,
    ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
    
    RETURN BinaryExp{exp, token, expression(rbp)}
```

###### Selector expression
A selector expression is exactly the same as a binary expression except the two values are separated by a `.`.

```
CASE PERIOD:
    RETURN SelectorExp{exp, expression(rbp)}
```

###### Call expression
A call expression is similar to a key value list expression.

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
Statement parses the token in the context of the start of a _statement_, instead of a expression as in `nud`. This is an important distinction since the same token can mean different things if it is at the start of an expression of statement.

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
To parse an if statements we look for the `IF` token, then parse the condition as an expression and the block as a block. If their is an `ELSE` after the block, we check for an `IF`. If their is an `IF` we recursively call `if()` to parse the next if, otherwise it must be a final else with no condition.

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
For statements are similar to if statements

```
CASE FOR:
    expect(FOR)
    index <- declaration()
    condition <- expression()
    body <- block()
```

###### Variable declaration
Variable declarations are normally handled elsewhere however if it starts with the optional `var` then we can parse the variable declaration now (rather than later).

```
CASE VAR:
    RETURN declaration()
```

###### Increment expression
Increment statements do not start with a keyword, so we look for an identifier. If their is not a proceeding increment or decrement token then we know the expression is a assignment or declaration node so we let the caller handle it.

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
Their are only two declaration nodes, functions and variables.

###### Function declaration
Function declaration are more complicated than most nodes since they contain a lot of information

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

###### Variable declaration
Variable declaration come in two forms, short form such as `foo := 123` and `var bar int = 199`. In the case of the long form declaration we know ahead of time that is a variable declaration since it starts with the var keyword.

```
CASE VAR:
    type <- Type()
    name <- Ident()
    value <- Expression()
    RETURN VariableDcl{type, name, value}
```

Otherwise the statement wrapper function first try's to parse it as a statement with `smtd()`, for short variable declarations this will return `NULL`. Next it try's parsing it as an expression, this will return a binary expression node where the operator is a assign or define type. From their the expression is converted into a statement.

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
Error handling inside a parser is particularly difficult, if their an unexpected token anywhere, we cant parse the current expression, statement or declaration however their may be other errors further down. Most compilers will no halt at a single error, but continue so that any other errors can be reported in a single compilation which makes finding and fixing bugs much easier.

So whenever we call `expect()` we return the token we were expecting or `NULL` if the token was not what we expected. If the token was incorrect an error is produced and added to the array of errors. It is up to the caller how to handle and incorrect token, generally their is three different options.

* If the token is non fatal such as a `::` in a function definition, the parser can continue from were it is.
* If we are inside a statement and the token was fatal we can skip over the tokens until a `SEMI` token is reached, then the parser can continue safely.
* Otherwise the whole block must be skipped.

If the parser doesn't skip over the affected tokens, the same line of source code would have many conflicting errors which wouldn't make sense.

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
#import "../src/includes/parser.h"
```

#### parser.c
This is the source file for the parser implementation
```
#import "../src/parser.c"
```

### IR Generation
Having converted the source code to tokens and the tokens to an abstract syntax tree we now can transform it into LLVM IR which LLVM will compile to machine code. LLVM IR is much lower level than Fur however the LLVM C API does a lot of the heavy lifting for by constructing the intermediary language in memory.

#### If statements
LLVM IR does not have any if statements only conditional branches. To branch we must break the code into separate blocks which end with a terminating statements (such as a branch, return etc). Since if statements can be chained it make sense for the transformation to be recursive hence we need a two functions, the recursive `compileIfBranch()` function and the `compileIf()` function to call into the recursive function.

`compileIf()` just sets up a `endBlock` which is the block which the program will continue executing from.
```
FUNCTION compileIf(irgen, ifSmt)
    endBlock <- LLVMAppendBasicBlock(irgen.function, "endBlock")
    compileIfBranch(irgen, ifSmt, _, endBlock)
ENDFUNCTION
```

`compileIfBranch()` takes four arguments: a reference to irgen which holds some state about the ir generation, the if node, the block to branch from (will be `NULL` for root level if branches) and the block to continue execution from. 

The first step is to check for a condition, if the condition is `NULL` then this branch must be an else branch therefore we compile the block and exit. `CompileBlockAt()` compiles a block statement and returns an `LLVMBlockRef` to were the execution should continue. We use `SetBlock()` to set it as are current block and check if its not terminated, if its not then we insert a branch to the end block.
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

Next we check for any chaining else if/else blocks. If their are we recursively call this function to compile the whole chain.
```
SetBlock(irgen, falseBlock)
IF ifSmt.else != _ THEN
    CompileIfBranch(irgen, ifSmt.else, falseBlock, endBlock)
ENDIF
```

Finally we set the end block as the current block so any further instructions execute from their.
```
SetBlock(irgen, endBlock)
```

#### irgen.h
This is the header file for the IR generation implementation
```
#import "../src/includes/irgen.h"
```

#### irgen.c
This is the source file for the IR generation implementation
```
#import "../src/irgen.c"
```

## Testing
When creating this project I used the test driven development workflow in which you write a test first, watch it fail, then write the code to make it parse. By following this I have constructed a test program which tests every unit of the compiler as well as integration tests to test the whole system. This is so important when building software to ensure no new code breaks any features. 

### String test
#### StringTest.CreateNewString 
Tests that new strings, from a C string, are constructed correctly
```
TEST(StringTest, CreateNewString) {
    string s = string_new("test");
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}
```

#### StringTest.CreateNewStringLength
Test that new strings, from a c string with a specified length, are constructed correctly
```
TEST(StringTest, CreateNewStringLength) {
    string s = string_new_length("testtesttest", 4);
    ASSERT_EQ(STRING_HEADER(s)->length, 4);
    ASSERT_EQ(STRING_HEADER(s)->capacity, 4);
    ASSERT_EQ(0, strcmp("test", s));
    string_free(s);
}
```

#### StringTest.CreateNewStringFile
Test that new strings, from a file handle, are constructed correctly
```
TEST(StringTest, CreateNewStringFile) {
    FILE *f = fopen("/tmp/string_test_file.fur", "w");
    fprintf(f, "test");
    fclose(f);

    f = fopen("/tmp/string_test_file.fur", "r");
    string s = string_new_file(f);
    fclose(f);

    ASSERT_EQ(0, strcmp(s, "test"));
}
```

#### Stringtest.CopyString
Test that a string is copyed correctly
```
TEST(StringTest, CopyString) {
    string s = string_new("test");
    string copy = string_copy(s);
    ASSERT_NE(s, copy);
    ASSERT_EQ(0, strcmp(s, copy));
}
```

#### StringTest.StringLength
Test that the correct length of a string is returned
```
TEST(StringTest, StringLength) {
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}
```

#### StringTest.StringCapacity
Test that a string returns the correct capacity
```
TEST(StringTest, StringCapacity) { 
    string s = string_new("test");
    ASSERT_EQ(4, string_length(s));
}
```

#### StringTest.StringAvalivle
Test that string returns the correct amount of space avalible
```
TEST(StringTest, StringAvalible) {
    string s = string_new("test");
    ASSERT_EQ(0, string_avalible(s));
}
```

#### StringTest.StringExpand
test that a string correctly expands its space
```
TEST(StringTest, StringExpand) {
    string s = string_new("test");
    s = string_expand(s, 10);
    ASSERT_EQ(10, string_capacity(s));
    ASSERT_EQ(6, string_avalible(s));
    ASSERT_EQ(4, string_length(s));
    ASSERT_EQ(0, strcmp(s, "test"));
}
```

#### StringTest.StringClear
Test that a string correctly clears its data
```
TEST(StringTest, StringClear) {
    string s = string_new("test");
    s = string_clear(s);
    ASSERT_EQ(0, strcmp(s, ""));
}
```

#### StringTest.StringAppend
Test that two strings are correctly appended
```
TEST(StringTest, StringAppend){
    string s1 = string_new("hello ");
    string s2 = string_new("world");
    s1 = string_append(s1, s2);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringAppendLength
Test that two strings, with a specified length, are correctly appended
```
TEST(StringTest, StringAppendLength) {
    string s1 = string_new("hello ");
    string s2 = string_new("world of earth");
    s1 = string_append_length(s1, s2, 5);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringAppendCString
Test that a string and a C String are appended correctly
```
TEST(StringTest, StringAppendCString) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringMultipleAppendWithMalloc
Test that a string reallocates correctly with a memory allocation in the middle
```
TEST(StringTest, StringMultipleAppendWithMalloc) {
    string s1 = string_new("hello");
    s1 = string_append_cstring(s1, (char *)" world");
    malloc(10);
    s1 = string_append_cstring(s1, (char *)" of");
    s1 = string_append_cstring(s1, (char *)" ours");

    ASSERT_EQ(0, strcmp(s1, "hello world of ours"));
}
```

#### StringTest.StringSliceEnd
Test that a string is correctly sliced from the end
```
TEST(StringTest, StringSliceEnd) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 0, 11);
    ASSERT_EQ(0, strcmp(s1, "hello world"));
}
```

#### StringTest.StringSlice
Test that a string is correctly sliced in the middle
```
TEST(StringTest, StringSlice) {
    string s1 = string_new("hello world of ours");
    s1 = string_slice(s1, 6, 11);
    ASSERT_EQ(0, strcmp(s1, "world"));
}
```

#### StringTest.StringEqual
Test that two equal strings are compared correctly
```
TEST(StringTest, StringEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar");
    ASSERT_EQ(true, string_equals(s1, s2));
}
```

#### StringTest.StringNotEqualLength
Test that two non equal lengthed strings are compared correctly
```
TEST(StringTest, StringNotEqualsLength) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bar bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}
```

#### StringTest.StringNotEqual
Test that tow equal lengthed strings are compared correctly
```
TEST(StringTest, StringNotEquals) {
    string s1 = string_new("foo bar");
    string s2 = string_new("foo bat");
    ASSERT_EQ(false, string_equals(s1, s2));
}
```

### Pool test

#### PoolTest.NewPool
Tests that a new pool is correctly constructed
```
TEST(PoolTest, NewPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.InsertPool
Tests that an item is correctly inserted into the pool
```
TEST(PoolTest, InsertPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int v1 = 123;
    int *e1 = (int *)pool_get(int_pool);
    memcpy(e1, &v1, sizeof(int));

    ASSERT_EQ(v1, *e1);
    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(1, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.FillPool
Tests the pool can be filled and `pool_full()` returns `true`
```
TEST(PoolTest, FillPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 5; i++) {
        int *e = (int *)pool_get(int_pool);
        memcpy(e, &i, sizeof(int));
    }

    ASSERT_TRUE(pool_full(int_pool));
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(5, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.ExtendPool
Tests the pool capacity can be extended
```
TEST(PoolTest, ExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    ASSERT_EQ(5, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_extend(int_pool, 10);
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(0, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### Pooltest.OverflowExtendPool
Tests inserting into a full pool extends the pool
```
TEST(PoolTest, OverflowExtendPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    for (int i = 0; i < 6; i++) {
        int *pool_element = (int *)pool_get(int_pool);
        memcpy(pool_element, &i, sizeof(int));
    }

    ASSERT_FALSE(pool_full(int_pool));
    ASSERT_EQ(10, pool_size(int_pool));
    ASSERT_EQ(6, pool_count(int_pool));
    pool_destroy(int_pool);
}
```

#### PoolTest.ReleaseElementFromPool
Tests an element is correctly released from the pool 
```
TEST(PoolTest, ReleaseElementFromPool) {
    pool *int_pool = new_pool(sizeof(int), 5);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));
    
    ASSERT_EQ(3, pool_count(int_pool));
    
    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));
    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);

    pool_destroy(int_pool);
}
```

#### PoolTest.ReusePool
Tests that memory can be reclaimed and reused from the pool
```
TEST(PoolTest, ReusePool) {
    pool *int_pool = new_pool(sizeof(int), 3);
    int *e1 = (int *)pool_get(int_pool);
    int v1 = 1;
    memcpy(e1, &v1, sizeof(int));

    int *e2 = (int *)pool_get(int_pool);
    int v2 = 2;
    memcpy(e2, &v2, sizeof(int));
    
    int *e3 = (int *)pool_get(int_pool);
    int v3 = 3;
    memcpy(e3, &v3, sizeof(int));

    ASSERT_EQ(3, pool_count(int_pool));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(3, *e3);

    pool_release(int_pool, e3);
    ASSERT_EQ(2, pool_count(int_pool));

    int *e4 = (int *)pool_get(int_pool);
    ASSERT_EQ(*e3, *e4);
    
    int v4 = 4;
    memcpy(e4, &v4, sizeof(int));

    ASSERT_EQ(1, *e1);
    ASSERT_EQ(2, *e2);
    ASSERT_EQ(4, *e4);
}
```

### Queue test
#### QueueTest.NewQueue
Tests that a new queue is correctly constructed
```
TEST(QueueTest, NewQueue) {
    queue *q = new_queue(sizeof(int));
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(sizeof(int) + sizeof(queue_item), q->element_size);
}
```

#### QueueTest.PushPopFrount
Tests that an element can be correctly pushed and popped from the frount
```
TEST(QueueTest, PushPopFront) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_front(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_front(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}
```

#### QueueTest.PushPopBack
Tests that an element can be correctly pushed and popped from the back
```
TEST(QueueTest, PushPopBack) {
    queue *q = new_queue(sizeof(int));
    int *in = (int *)queue_push_back(q);
    int value = 100;
    memcpy(in, &value, sizeof(int));
    ASSERT_EQ(1, queue_size(q));

    int *out = (int *)queue_pop_back(q);
    ASSERT_EQ(0, queue_size(q));
    ASSERT_EQ(in, out);
    ASSERT_EQ(*in, *out);
    queue_free_item(out);
}
```

#### QueueTest.PushPopManyFrount
Tests that many elements can be pushed and popped off the front in the correct order
```
TEST(QueueTest, PushPopManyFront) {
    queue *q = new_queue(sizeof(int));
    for(int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_back(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 0; i < 100; i++) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
```

#### QueueTest.PushPopManyBack
Tests that many elements can be pushed and popped off the back in the correct order
```
TEST(QueueTest, PushPopManyBack) {
    queue *q = new_queue(sizeof(int));
    for (int i = 0; i < 100; i++) {
        int *in = (int *)queue_push_front(q);
        memcpy(in, &i, sizeof(int));
        ASSERT_EQ(i+1, queue_size(q));
    }

    for(int i = 99; i >= 0; i--) {
        int *out = (int *)queue_pop_front(q);
        ASSERT_EQ(i, *out);
        queue_free_item(out);
    }
}
```

### Lexer test

#### LexerTest.Identifier
Test that identifiers are lexed correctly
```
TEST(LexerTest, Identifier) {

    tcase cases[] = {
        tcase{"test", IDENT, "test"},
        tcase{"a", IDENT, "a" },
        tcase{"test123", IDENT, "test123"},
    };

    for (int i = 0; i < sizeof(cases) / sizeof(tcase); i++) {
        tcase c = cases[i];

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Numbers
Test that integer numbers are lexed correctly
```
TEST(LexerTest, Numbers) {
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

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Strings
Test that strings are lexed correctly
```
TEST(LexerTest, Strings) {
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

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.Symbols
Tests that all symbols are lexed correctly
```
TEST(LexerTest, Symbols) {
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

        Token *tokens = Lex((char *)c.input);

        ASSERT_STREQ(TokenName(c.expectedType), TokenName(tokens[0].type));
        ASSERT_STREQ(c.expectedValue, tokens[0].value);
        ASSERT_STREQ(TokenName(END), TokenName(tokens[1].type));
    }
}
```

#### LexerTest.LineNumbers
Tests the line numbers of the tokens are correct
```
TEST(LexerTest, LineNumbers) {
    Token *tokens = Lex((char *)"1\n2\n3");
    
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(i+1, tokens[i].line);	
    }
}
```

#### LexerTest.ColumnNumbers
Tests the column numbers of the tokens are correct
```
TEST(LexerTest, ColumnNumbers) {
    Token *tokens = Lex((char *)"foo bar baz");

    ASSERT_EQ(1, tokens[0].column);
    ASSERT_EQ(5, tokens[1].column);
    ASSERT_EQ(9, tokens[2].column);
}
```

#### LexerTest.SemiColonInsertion
Tests that semi colons are inserted correctly
```
TEST(LexerTest, SemiColonInsertion) {
    Token *tokens = Lex((char *)"foo\nbar");
    ASSERT_STREQ(TokenName(SEMI), TokenName(tokens[1].type));
}
```

### Parser test

#### ParserTest.ScopeEnter
Test that a new scope is correctly entered
```
TEST(ParserTest, ScopeEnter) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    ASSERT_EQ(outer->outer, NULL);
    parser_enter_scope(p);
    ASSERT_TRUE(outer == p->scope->outer);
}
```

#### ParserTest.ScopeExit
Tests that a scope is correctly exited
```
TEST(ParserTest, ScopeExit) {
    parser *p = new_parser(NULL);
    scope *outer = p->scope;
    parser_enter_scope(p);
    parser_exit_scope(p);
    ASSERT_TRUE(outer == p->scope);
}
```

#### ParserTest.ScopeInsert
Tests that a new object is correctly inserted into the scope
```
TEST(ParserTest, ScopeInsert) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = new_argument_dcl(p->ast, NULL, (char *)"test_name");
    bool inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_TRUE(inserted);

    scope_object *found;
    HASH_FIND_STR((scope_object *)p->scope->objects, (char *)"test", found);
    ASSERT_STREQ(obj->name, found->obj->name);
    ASSERT_STREQ(obj->node->argument.name, 
        (char *)found->obj->node->argument.name);			

    inserted = parser_insert_scope(p, (char *)"test", obj);
    ASSERT_FALSE(inserted);
}
```

#### ParserTest.ScopeFind
Tests that an object can be found in scope
```
TEST(ParserTest, ScopeFind) {
    parser *p = new_parser(NULL);
    Object *obj = (Object *)malloc(sizeof(Object));
    obj->type = badObj;
    obj->name = (char *)"test";
    obj->node = NULL;
    parser_insert_scope(p, (char *)"test", obj);

    // Enter and exit some scopes
    parser_enter_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);
    parser_enter_scope(p);
    parser_exit_scope(p);

    Object *found = parser_find_scope(p, (char *)"test");
    ASSERT_EQ(obj->name, found->name); // pointer should be same

    found = parser_find_scope(p, (char *)"not here");
    ASSERT_EQ(found, NULL);
}
```

#### ParserTest.ParseLiteralExpression
Tests that a literal expression is parsed correctly
```
TEST(ParserTest, ParseLiteralExpression) {
    Exp *exp = parse_expression_from_string((char *)"123");

    ASSERT_FALSE(exp == NULL);
    ASSERT_EQ((int)literalExp, (int)exp->type);
    ASSERT_STREQ("123", exp->literal.value);
}
```

#### ParserTest.ParseIdentExpression
Tests that a identifier expression is parsed correcly
```
TEST(ParserTest, ParseIdentExpression) {
    Exp *exp = parse_expression_from_string((char *)"test");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test", exp->ident.name);
}
```

#### ParserTest.ParseIdentExpressionWithNumber
Tests that a identifier expression, with a number, is parsed correctly
```
TEST(ParserTest, ParseIdentExpressionWithNumber) {
    Exp *exp = parse_expression_from_string((char *)"test123");

    ASSERT_EQ((int)identExp, (int)exp->type);
    ASSERT_STREQ("test123", exp->ident.name);
}
```

#### ParserTest.ParseBinaryExpression
Tests that a binary expression is parsed correctly
```
TEST(ParserTest, ParseBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
}
```

#### ParserTest.ParseBidmasBinaryExpression
Tests that the rules of BIDMAS when parsing binary expressions are followed
```
TEST(ParserTest, ParseBidmasBinaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"a + b * c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)ADD, (int)exp->binary.op.type);
    ASSERT_EQ((int)MUL, (int)exp->binary.right->binary.op.type);
}
```

#### ParserTest.ParseSelectorExpression
Tests that a selector expression is correctly parsed
```
TEST(ParserTest, ParseSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.selector->ident.name);
}
```

#### ParserTest.ParseDoubleSelectorExpression
Tests that a double selector expression is correclty parsed
```
TEST(ParserTest, ParseDoubleSelectorExpression) {
    Exp *exp = parse_expression_from_string((char *)"a.b.c");

    ASSERT_EQ((int)selectorExp, (int)exp->type);
    ASSERT_EQ((int)selectorExp, (int)exp->selector.exp->type);
    ASSERT_STREQ((char *)"a", exp->selector.exp->selector.exp->ident.name);
    ASSERT_STREQ((char *)"b", exp->selector.exp->selector.selector->ident.name);
    ASSERT_STREQ("c", exp->selector.selector->ident.name);
}
```

#### ParserTest.ParseIndexExpression
Tests that an index expression is correctly parsed
```
TEST(ParserTest, ParseIndexExpression) {
    Exp *exp = parse_expression_from_string((char *)"test[1]");

    ASSERT_EQ((int)indexExp, (int)exp->type);
}
```

#### ParserTest.ParserightAssociativeBinaryOperators
Tests that right associative binary operators are parsed correctly
```
TEST(ParserTest, ParseRightAssociativeBinaryOperators) {
    Exp *exp = parse_expression_from_string((char *)"a || b || c");

    ASSERT_EQ((int)binaryExp, (int)exp->type);
    ASSERT_EQ((int)binaryExp, (int)exp->binary.right->type);
}
```

#### ParserTest.ParseUnaryExpression
Tests that a unary expression is parsed correctly
```
TEST(ParserTest, ParseUnaryExpression) {
    Exp *exp = parse_expression_from_string((char *)"!a");
    
    ASSERT_EQ((int)unaryExp, (int)exp->type);
}
```

#### ParserTest.ParseUnaryMinusExpression
Tests that a unary minus expression works correctly
```
TEST(ParserTest, ParseUnaryMinusExpression) {
    Exp *exp = parse_expression_from_string((char *)"-a");

    ASSERT_EQ((int)unaryExp, (int)exp->type);
}
```

#### ParserTest.ParseAssignmentOperator
Tests that a assignment expression works correctly
```
TEST(ParserTest, ParseAssignmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a = b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}
```

#### ParserTest.ParseAddAssigmentOperator
Tests that a add assignment expression is parsed correctly
```
TEST(ParserTest, ParseAddAssigmentOperator) {
    Smt *smt = parse_statement_from_string((char *)"a += b");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
    ASSERT_EQ((int)binaryExp, (int)smt->assignment.right->type);
    ASSERT_EQ((int)ADD, (int)smt->assignment.right->binary.op.type);
    ASSERT_STREQ((char *)"a", smt->assignment.right->binary.left->ident.name);
    ASSERT_STREQ((char *)"b", smt->assignment.right->binary.right->ident.name);
}
```

#### ParserTest.ParseReturnStatment
Tests that a return statement is parsed correctly
```
TEST(ParserTest, ParseReturnStatment) {
    Smt *smt = parse_statement_from_string((char *)"return a");

    ASSERT_EQ((int)returnSmt, (int)smt->type);
}
```

#### ParserTest.ParseBlockStatment
Tests that a block statement is parsed correctly
```
TEST(ParserTest, ParseBlockStatment) {
    Smt *smt = parse_statement_from_string((char *)"{\nreturn test\n}");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);
}
```

#### ParserTest.ParseBlockSingleLine
Tests that a single line block is parsed correctly
```
TEST(ParserTest, ParserBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(1, smt->block.count);
    ASSERT_EQ((int)returnSmt, (int)smt->block.smts->type);    
}
```

#### ParserTest.ParseLongBlockSingleLine
Tests that a single line block with many statements is parsed correctly
```
TEST(ParserTest, ParserLongBlockSingleLine) {
    Smt *smt = parse_statement_from_string((char *)"{ a = 1; b = 2; return test }");

    ASSERT_EQ((int)blockSmt, (int)smt->type);
    ASSERT_EQ(3, smt->block.count);
}
```

#### ParserTest.ParseIfStatment
Tests that an if statement is parsed correctly
```
TEST(ParserTest, ParseIfStatment) {
    Smt *smt = parse_statement_from_string((char *)"if true {\nreturn false\n}");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_EQ(smt->ifs.elses, NULL);
}
```

#### ParserTest.ParseIfElseStatment
Tests that an if else statement is parsed correctly
```
TEST(ParserTest, ParseIfElseStatement) {
    Smt *smt = parse_statement_from_string((char *)"if true { return 123 } else { return 321 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elses = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}
```

#### ParserTest.ParseIfElseIfElseStatment
Tests that an if else if else statement is parsed correctly
```
TEST(ParserTest, ParseIfElseIfElseStatment) {
    Smt *smt = parse_statement_from_string((char *)"if false { return 321 } else if true { return 123 } else { return 0 }");

    ASSERT_EQ((int)ifSmt, (int)smt->type);
    ASSERT_EQ((int)identExp, (int)smt->ifs.cond->type);
    ASSERT_EQ((int)blockSmt, (int)smt->ifs.body->type);
    ASSERT_NE(smt->ifs.elses, NULL);

    Smt *elif = smt->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elif->type);
    ASSERT_NE(elif->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elif->ifs.body->type);
    ASSERT_NE(elif->ifs.elses, NULL);

    Smt *elses = elif->ifs.elses;
    ASSERT_EQ((int)ifSmt, (int)elses->type);
    ASSERT_EQ(elses->ifs.cond, NULL);
    ASSERT_EQ((int)blockSmt, (int)elses->ifs.body->type);
    ASSERT_EQ(elses->ifs.elses, NULL);
}
```

#### ParserTest.ParseShortVaribleDeclare
Tests that a short variable declaration is parsed correctly
```
TEST(ParserTest, ParserShortVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"a := 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_TRUE(obj->node == smt->declare);
}
```

#### ParserTest.ParseLongVaribleDeclare
Tests that a long variable declaration is parsed correctly
```
TEST(ParserTest, ParseLongVaribleDeclare) {
    parser *p = new_parser(Lex((char *)"var int a = 10"));
    Smt *smt = parse_statement(p);

    ASSERT_EQ((int)declareSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->declare->type);
    ASSERT_STREQ("a", smt->declare->varible.name);
    ASSERT_EQ((int)identExp, (int)smt->declare->varible.type->type);
    ASSERT_STREQ("int", smt->declare->varible.type->ident.name);

    Object *obj = parser_find_scope(p, (char *)"a");
    ASSERT_NE(obj, NULL);
    ASSERT_TRUE(obj->node == smt->declare);
}
```

#### ParserTest.ParseArrayType
Tests that an array type is parsed correctly
```
TEST(ParserTest, ParseArrayType) {
    const char *src = "int[3]";
    parser *p = new_parser(Lex((char *)src));
    Exp *type = parse_type(p);

    ASSERT_EQ((int)arrayTypeExp, (int)type->type);
}
```

#### ParserTest.ParseFunctionDefinition
Tests that a function definition is parsed correctly
```
TEST(ParserTest, ParseFunctionDefinition) {
    const char *src = "proc test :: int a, int b -> int {\nreturn a + b\n}";
    parser *p = new_parser(Lex((char *)src));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ((int)functionDcl, (int)dcl->type);
    ASSERT_EQ(2, (int)dcl->function.argCount);
    ASSERT_EQ((int)identExp, (int)dcl->function.returnType->type);

    Object *obja = parser_find_scope(p, (char *)"a");
    Object *objb = parser_find_scope(p, (char *)"b");
    
    ASSERT_NE(obja, NULL);
    ASSERT_NE(objb, NULL);
    ASSERT_NE(obja->node, objb->node);
    ASSERT_EQ(dcl->function.args, obja->node);
    ASSERT_EQ(dcl->function.args + 1, objb->node);
}
```

#### ParserTest.ParseEmptyCallExpression
Tests that a call expression is parsed correctly
```
TEST(ParserTest, ParseEmptyCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test()");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(0, exp->call.argCount);
}
```

#### ParserTest.ParseCallExpression
Tests that a call expression is parsed correctly
```
TEST(ParserTest, ParseCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(1, test)");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(2, exp->call.argCount);

    ASSERT_STREQ("1", exp->call.args[0].literal.value);
}
```

#### ParserTest.ParseCallInCallExpression
Tests that a call in a call expression is parsed correctly
```
TEST(ParserTest, ParseCallInCallExpression) {
    Exp *exp = parse_expression_from_string((char *)"test(test())");
    
    ASSERT_EQ((int)callExp, (int)exp->type);
    ASSERT_EQ(1, exp->call.argCount);
}
```

#### ParserTest.ParseForLoop
Tests that a for loop is parsed correctly
```
TEST(ParserTest, ParseForLoop) {
    Smt *smt = parse_statement_from_string((char *)"for i := 0; i < 10; i += 1 {}");

    ASSERT_EQ((int)forSmt, (int)smt->type);
    ASSERT_EQ((int)varibleDcl, (int)smt->fors.index->type);
    ASSERT_EQ((int)binaryExp, (int)smt->fors.cond->type);
    ASSERT_EQ((int)assignmentSmt, (int)smt->fors.inc->type);
    ASSERT_EQ((int)blockSmt, (int)smt->fors.body->type);
}
```

#### ParserTest.ParseIncrement
Tests that a increment statement is parsed correctly
```
TEST(ParserTest, ParseIncrement) {
    Smt *smt = parse_statement_from_string((char *)"i++");

    ASSERT_EQ((int)assignmentSmt, (int)smt->type);
}
```

#### ParserTest.ParseKeyValueList
Tests that a key value list is parsed correctly
```
TEST(ParserTest, ParseKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{a: 1, b: 2}");

    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(2, exp->keyValueList.keyCount);
    ASSERT_STREQ("a", exp->keyValueList.keyValues[0].keyValue.key->ident.name);
    ASSERT_STREQ("b", exp->keyValueList.keyValues[1].keyValue.key->ident.name);
}
```

#### ParserTest.ParseEmptyKeyValueList
Tests that an empty key value list is parsed correctly
```
TEST(ParserTest, ParseEmptyKeyValueList) {
    Exp *exp = parse_expression_from_string((char *)"{}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(0, exp->keyValueList.keyCount);
}
```

#### ParserTest.ParseNullKeyValueList
Tests that an key value list with null keys is parsed correctly
```
TEST(ParserTest, ParseNullKeyValueList) {   
    Exp *exp = parse_expression_from_string((char *)"{1, 2, 3}");
    
    ASSERT_EQ((int)keyValueListExp, (int)exp->type);
    ASSERT_EQ(3, exp->keyValueList.keyCount);
}
```

#### ParserTest.ParseArrayExpression
Tests that an array initilizer expression is parsed correctly
```
TEST(ParserTest, ParseArrayExpression) {
    Exp *exp = parse_expression_from_string((char *)"[1, 2, 3]");

    ASSERT_EQ((int)arrayExp, (int)exp->type);
    ASSERT_EQ(3, exp->array.valueCount);
}
```

#### ParserTest.ParseFunctionDclWithoutProc
Tests that a function declaration is parsed without a proc (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutProc) {
    parser *p = new_parser(Lex((char *)"add :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(PROC, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutName
Tests that a function declaration is parsed without a name (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutName) {
    parser *p = new_parser(Lex((char *)"proc :: -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutArgumentSeperator
Tests that a function declaration without an argument seperator (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutArgumentSeperator) {
    parser *p = new_parser(Lex((char *)"proc add -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(DOUBLE_COLON, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutCommas
Tests that a function declaration without commas are passed correctly (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutCommas) {
    parser *p = new_parser(Lex((char *)"proc add :: int a int b int c -> int {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(2, queue_size(p->error_queue));
    
    for (int i = 0; i < 2; i++) {
        parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
        ASSERT_EQ(parser_error_expect_token, error->type);
        ASSERT_EQ(1, error->length);
        ASSERT_EQ(COMMA, error->expect_token.type);
    }
}
```

#### ParserTest.ParseFunctionDclWithoutArgTypeOrName
Tests that a function declaration without argument type or name is parsed correctly (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutArgTypeOrName) {
    parser *p = new_parser(Lex((char *)"proc add :: int, int a -> int {}"));
    Dcl *dcl = parse_function_dcl(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseFunctionDclWithoutArrow
Tests that a function declaration without an arrow is parsed correctly (and error is produced)
```
TEST(ParserTest, ParseFunctionDclWithoutArrow) {
    parser *p = new_parser(Lex((char *)"proc add :: int a {}"));
    Dcl *dcl = parse_function_dcl(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));
    
    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ARROW, error->expect_token.type);
}
```

#### ParserTest.ParseNonDeclaration
Tests that an error is produced when a declaration is expected
```
TEST(ParserTest, ParseNonDecleration) {
    parser *p = new_parser(Lex((char *)"return a"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_declaration, error->type);
    ASSERT_EQ(1, error->length);
}
```

#### ParserTest.ParseVaribleDclWithoutType
Tests that a long variable declaration without a type is parsed correclty (and error is produced)
```
TEST(ParserTest, ParseVaribleDclWithoutType) {
    parser *p = new_parser(Lex((char *)"var = 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_type, error->type);
    ASSERT_EQ(1, error->length);
}
```

#### ParserTest.ParseVariableDclWithoutName
Tests that an error is produced when a varible declaration doesnt have a name
```
TEST(ParserTest, ParseVariableDclWithoutName) {
    parser *p = new_parser(Lex((char *)"var int = 100"));
    Dcl *dcl = parse_declaration(p);
    
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(IDENT, error->expect_token.type);
}
```

#### ParserTest.ParseVaribleDclWithoutEquals
Tests that a long variable declaration without an equals is parsed correctly (and error is produced)
```
TEST(ParserTest, ParseVaribleDclWithoutEquals) {
    parser *p = new_parser(Lex((char *)"var int a 100"));
    Dcl *dcl = parse_declaration(p);

    ASSERT_NE(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_token, error->type);
    ASSERT_EQ(1, error->length);
    ASSERT_EQ(ASSIGN, error->expect_token.type);
}
```

#### ParserTest.ParseVaribleDclWithoutValue
Tests that a long variable declaration without a value produces error
```
TEST(ParserTest, ParseVaribleDclWithoutValue) {
    parser *p = new_parser(Lex((char *)"var int a = "));
    Dcl *dcl = parse_declaration(p);
    ASSERT_EQ(NULL, dcl);
    ASSERT_EQ(1, queue_size(p->error_queue));

    parser_error *error = (parser_error *)queue_pop_front(p->error_queue);
    ASSERT_EQ(parser_error_expect_prefix, error->type);
    ASSERT_EQ(1, error->length);
}
```

### Irgen tests
```
#define TEST_TYPE(name, src, expectedType) TEST(IrgenTest, name){       \
    parser *p = new_parser(Lex((char *)src));                           \
    Exp *e = parse_type(p);                                             \
    Irgen *irgen = NewIrgen();                                          \
    LLVMTypeRef type = CompileType(e);                                  \
    ASSERT_TRUE(type == expectedType);                                  \
}        

#define TEST_LITERAL(name, src, expectedType, expectedValue) TEST(IrgenTest, name) {    \
    parser *p = new_parser(Lex((char *)src));                                           \
    Exp *e = parse_expression(p, 0);                                                    \
    Irgen *irgen = NewIrgen();                                                          \
    LLVMValueRef value = CompileLiteralExp(irgen, e);                                   \
    ASSERT_TRUE(LLVMIsConstant(value));                                                 \
    ASSERT_STREQ(expectedValue, LLVMPrintValueToString(value));                         \
}

#define TEST_CAST(name, value, cast) TEST(IrgenTest, name) {                        \
    Irgen *irgen = NewIrgen();                                                      \
    irgen->function = LLVMAddFunction(irgen->module, "test", LLVMInt64Type());      \
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");       \
    irgen->builder = LLVMCreateBuilder();                                           \
    LLVMPositionBuilderAtEnd(irgen->builder, entry);                                \
    LLVMValueRef castValue = Cast(irgen, value, cast);                              \
    ASSERT_EQ(LLVMTypeOf(castValue), cast);                                         \
} 
```

#### IrgenTest.CompileTypeInt
Tests integer type is compiled correctly
```
TEST_TYPE(CompileTypeInt, "int", LLVMInt64Type())
```

#### IrgenTest.CompileTypeI64
Tests 64 bit integer type is compiled correctly
```
TEST_TYPE(CompileTypeI64, "i64", LLVMInt64Type())
```

#### IrgenTest.CompileTypeI32
Tests 32 bit integer type is compiled correctly
```
TEST_TYPE(CompileTypeI32, "i32", LLVMInt32Type())
```

#### IrgenTest.CompileTypeI16
Tests 16 bit integer type is compiled correctly
```
TEST_TYPE(CompileTypeI16, "i16", LLVMInt16Type())
```

#### IrgenTest.CompileTypeI8
Tests 8 bit integer type is compiled correctly
```
TEST_TYPE(CompileTypeI8, "i8", LLVMInt8Type())
```

#### IrgenTest.CompileTypeFloat
Tests float type is compiled correctly
```
TEST_TYPE(CompileTypeFloat, "float", LLVMFloatType())
```

#### IrgenTest.CompileTypeF64
Tests 64 bit float type is compiled correctly
```
TEST_TYPE(CompileTypeF64, "f64", LLVMDoubleType())
```

#### IrgenTest.CompileTypeF32
Tests 32 bit float type is compiled correctly
```
TEST_TYPE(CompileTypeF32, "f32", LLVMFloatType())
```

#### IrgenTest.CompileTypeIntArray
Tests integer array is compiled correctly
```
TEST_TYPE(CompileTypeIntArray, "int[3]", LLVMArrayType(LLVMInt64Type(), 3))
```

#### IrgenTest.CompileTypeFloatArray
Tests float array is compiled correctly
```
TEST_TYPE(CompileTypeFloatArray, "float[100]", LLVMArrayType(LLVMFloatType(), 100))
```

#### IrgenTest.CompileLiteralInt
Tests integer literal are compiled correctly
```
TEST_LITERAL(CompileLiteralInt, "123", LLVMInt64Type(), "i64 123")
```

#### Irgentest.CompileLiteralFloat
Tests float literals are compiled correctly
```
TEST_LITERAL(CompileLiteralFloat, "123.321", LLVMFloatType(), "float 0x405ED48B40000000")
```

#### IrgenTest.CompileLiteralHex
Tests hex literals are compiled correctly
```
TEST_LITERAL(CompileLiteralHex, "0x1000", LLVMInt64Type(), "i64 4096")
```

#### IrgenTest.CompileLiteralOctal
Tests octal literals are compiled correctly
```
TEST_LITERAL(CompileLiteralOctal, "0123", LLVMInt64Type(), "i64 83")
```

#### IrgenTest.I64ToI32
Tests cast from 64 bit integer to 32 bit integer
```
TEST_CAST(I64ToI32, LLVMConstIntOfString(LLVMInt64Type(), "123", 10), LLVMInt32Type())
```

#### IrgenTest.I16ToFloat
Tests cast from 16 bit integer to float
```
TEST_CAST(I16ToFloat, LLVMConstIntOfString(LLVMInt16Type(), "32", 10), LLVMFloatType())
```

#### IrgenTest.FloatToDouble
Tests cast from float to double
```
TEST_CAST(FloatToDouble, LLVMConstRealOfString(LLVMFloatType(), "1.42"), LLVMDoubleType())
```

#### IrgenTest.DoubleToI64
Tests cast from double to 64 bit integer
```
TEST_CAST(DoubleToI64, LLVMConstRealOfString(LLVMDoubleType(), "1245.12"), LLVMInt64Type())
```

### Integration tests
```
LLVMGenericValueRef intArg(int n) {
    return LLVMCreateGenericValueOfInt(LLVMInt64Type(), n, false);
}

int runLLVMModule(Irgen *irgen) {

    // create an execution engine   
    LLVMExecutionEngineRef engine;
    char *error = (char *)NULL;

    # if 1
        // Initialize jit
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

    LLVMValueRef mainFunc; 
    LLVMFindFunction(engine, "main", &mainFunc);

    int res = LLVMRunFunctionAsMain(engine, mainFunc, 0, NULL, NULL);

    LLVMDisposeExecutionEngine(engine);
    return res;
}

void TEST_ERROR(char *src, TokenType type) {
    parser *p = new_parser(Lex(src));
    ast_unit *f = parse_file(p);
    
    ASSERT_EQ(queue_size(p->error_queue), 1);
    parser_error *err = (parser_error *)queue_pop_back(p->error_queue);
    ASSERT_EQ(err->expect_token.type, type);
    error(
        src, 
        err->start->line, 
        err->start->column, 
        err->start->column + err->length, 
        (char *)"Expected token %s", 
        TokenName(err->expect_token.type));
}

void TEST_MODULE(char *src, int out) {
    /* generate module */
    parser *p = new_parser(Lex(src));
    ast_unit *f = parse_file(p);
    
    Irgen *irgen = NewIrgen();
    
    for (int i = 0; i < f->dclCount; i++) {
        CompileFunction(irgen, f->dcls[i]);
    }

    // LLVMDumpModule(irgen->module);

    /* check for errors in module */
    char *error = (char *)NULL;
    LLVMVerifyModule(irgen->module, LLVMPrintMessageAction, &error);
    LLVMDisposeMessage(error);

    /* run the function */
    int res = runLLVMModule(irgen);
    ASSERT_EQ(out, res);

    /* dispose of builder */
    LLVMDisposeBuilder(irgen->builder);
}

char *loadTest(std::string name) {
    // build path to the file
    char *cname = (char *)name.c_str();
    char *pre = (char *)"../tests/tests/";
    int pathLen = strlen(pre) + strlen(cname) + 1;
    char *path = alloca(pathLen);
    sprintf(path, "%s%s", pre, cname);
    // printf("Path: %s\n", path);
    
    // open file
    FILE *f = fopen(path, "rb");
    assert(f != NULL);

    // get the length of the file
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    // read file into buffer
    char *buffer = (char *)malloc(length + 1);
    fread(buffer, 1, length, f);
    fclose(f);
    buffer[length] = '\0';
    
    return buffer;
}
```

#### IntegrationTest.CompileFunctionLiteral
Tests `literal.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionLiteral){ 
    TEST_MODULE(loadTest("literal.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionBinaryInt
Tests `binaryInt.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionBinaryInt){ 
    TEST_MODULE(loadTest("binaryInt.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionBinaryFloat
Tests `binaryFloat.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionBinaryFloat){ 
    TEST_MODULE(loadTest("binaryFloat.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionLongVar
Tests `longVar.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionLongVar){ 
    TEST_MODULE(loadTest("longVar.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionShortVar
Tests `shortVar.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionShortVar){ 
    TEST_MODULE(loadTest("shortVar.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionIf
Tests `if.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionIf){ 
    TEST_MODULE(loadTest("if.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionIfElse
Tests `ifElse.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionIfElse){ 
    TEST_MODULE(loadTest("ifElse.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionIfElseIfElse
Tests `ifElseIfelse.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionIfElseIfElse){ 
    TEST_MODULE(loadTest("ifElseIfElse.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionIfElseIfelseIfElse
Tests `ifElseIfElseIfElse.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionIfElseIfElseIfElse){ 
    TEST_MODULE(loadTest("ifElseIfElseIfElse.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionFor
Tests `for.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionFor){ 
    TEST_MODULE(loadTest("for.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionArrayInit
Tests `arrayInit.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionArrayInit){ 
    TEST_MODULE(loadTest("arrayInit.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionAdd
Tests `add.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionAdd){ 
    TEST_MODULE(loadTest("add.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionUnary
Tests `unary.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionUnary){ 
    TEST_MODULE(loadTest("unary.fur"), 123); 
}
```

#### IntegrationTest.CompileFunctionReassignArg
Tests `reassignArg.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionReassignArg){ 
    TEST_MODULE(loadTest("reassignArg.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionGCD
Tests `gcd.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionGCD){ 
    TEST_MODULE(loadTest("gcd.fur"), 139);
}
```

#### IntegrationTest.CompileFunctionFibbonanci
Tests `fibbonanci.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionFibbonanci) {
    TEST_MODULE(loadTest("fibbonanci.fur"), 144);
}
```

#### IntegrationTest.CompileFunctionArraySum
Tests `arraySum.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionArraySum) {
    TEST_MODULE(loadTest("arraySum.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionNestedFor
Tests `nestedFor.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionNestedFor) {
    TEST_MODULE(loadTest("nestedFor.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionBubblesort
Tests `bubblesort.fur` compiles and returns the correct code
```
TEST(IntegrationTest, CompileFunctionBubblesort) {
    TEST_MODULE(loadTest("bubblesort.fur"), 123);
}
```

#### IntegrationTest.CompileFunctionProcColonError
Tests that functions without a double colon produce the correct error

```
#import "../tests/tests/procColonError.fur"
```

```
TEST(IntegrationTest, CompileFunctionProcColonError) {
    TEST_ERROR(loadTest("procColonError.fur"), DOUBLE_COLON);
}
```

![](https://i.imgur.com/x0Eli3B.png)

#### IntegrationTest.CompileFunctionProcArrowError
Tests that functions without an arrow prouduces the correct error

```
#import "../tests/tests/procArrowError.fur"
```

```
TEST(IntegrationTest, CompileFunctionProcArrowError) {
    TEST_ERROR(loadTest("procArrowError.fur"), ARROW);
}
```

![](https://i.imgur.com/9jX8bOH.png)

#### IntegrationTest.CompileFunctionProcNameError
Tests that fuunctions without a name prouduces the correct error

```
#import "../tests/tests/procNameError.fur"
```

```
TEST(IntegrationTest, CompileFunctionProcNameError) {
    TEST_ERROR(loadTest("procNameError.fur"), IDENT);
}
```

![](https://i.imgur.com/tyM40HK.png)

## Evaluation
In the analysis I stated that "simple algorithms like the greatest common divisor, bubble sort and Fibonacci sequence should be able to be made in Fur. Each of these algorithms are included as part of the integrations tests, which all pass, so I would say the final program meets the original requirements.

### Syntax objectives
As my tests show all syntax objectives are successfully understood by the parser, including all definition, statements and expressions.

### Memory management objectives
Since their is no run time at all, the is no managed memory, thus the memory management objective has been met

### Command line interface objectives
All flags were used during development to debug the compiler, so all flags exist and the objectives have been met.

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

## Technical solution

### build/
This folder is were the make file produces the executable and library for the project.

#### CMakeLists.txt
This is the make file for the project
```
#import "../build/CMakeLists.txt"
```

### src/
This is the folder were all the source files for the compiler reside.

#### includes/all.h
Contains a few helpfull defines for the whole project.
```
#import "../src/includes/all.h"
```

#### includes/ast.h
Typedefs for all the ast nodes.
```
#import "../src/includes/ast.h"
```

#### ast.c
Constructors for the AST nodes.
```
#import "../src/ast.c"
```

#### includes/error.h
```
#import "../src/includes/error.h"
```

#### error.c
Pretty prints errors to the terminal.
```
#import "../src/error.c"
```

#### includes/irgen.h
```
#import "../src/includes/irgen.h"
```

#### irgen.c
```
#import "../src/irgen.c"
```

#### includes/lexer.h
```
#import "../src/includes/lexer.h"
```

#### lexer.c
```
#import "../src/lexer.c"
```

#### lib.c
Includes all the source files so the compiler can be exposed as a library (which we use in the unit tests).
```
#import "../src/lib.c"
```

#### main.c
Entry point for the compiler
```
#import "../src/main.c"
```

#### includes/parser.h
```
#import "../src/includes/parser.h"
```

#### parser.c
```
#import "../src/parser.c"
```

#### includes/pool.h
```
#import "../src/includes/pool.h"
```

#### pool.c
```
#import "../src/pool.c"
```

#### includes/queue.h
```
#import "../src/includes/queue.h"
```

#### queue.c
```
#import "../src/queue.c"
```

#### includes/string.h
```
#import "../src/includes/string.h"
```

#### string.c
```
#import "../src/string.c"
```

#### includes/uthash.h
The only other external dependency (apart from LLVM), uthash which is a single header hash table.
```
#import "../src/includes/uthash.h"
```

### tests/
This folder contains the C++ program that tests the compiler

#### tests/add.fur
```
#import "../tests/tests/add.fur"
```

#### tests/arrayInit.fur
```
#import "../tests/tests/arrayInit.fur"
```

#### tests/arraySum.fur
```
#import "../tests/tests/arraySum.fur"
```

#### tests/binaryFloat.fur
```
#import "../tests/tests/binaryFloat.fur"
```

#### tests/binaryInt.fur
```
#import "../tests/tests/binaryInt.fur"
```

#### tests/bubbleSort.fur
```
#import "../tests/tests/bubblesort.fur"
```

#### tests/fibbonanci.fur
```
#import "../tests/tests/fibbonanci.fur"
```

#### tests/for.fur
```
#import "../tests/tests/for.fur"
```

#### tests/gcd.fur
```
#import "../tests/tests/gcd.fur"
```

#### tests/if.fur
```
#import "../tests/tests/if.fur"
```

#### tests/ifElse.fur
```
#import "../tests/tests/ifElse.fur"
```

#### tests/ifElseIfElse.fur
```
#import "../tests/tests/ifElseIfElse.fur"
```

#### tests/ifElseIfElseIfElse.fur
```
#import "../tests/tests/ifElseIfElseIfElse.fur"
```

#### tests/literal.fur
```
#import "../tests/tests/literal.fur"
```

#### tests/longVar.fur
```
#import "../tests/tests/longVar.fur"
```

#### tests/nestedFor.fur
```
#import "../tests/tests/nestedFor.fur"
```

#### tests/reassignArg.fur
```
#import "../tests/tests/reassignArg.fur"
```

#### tests/shortVar.fur
```
#import "../tests/tests/shortVar.fur"
```

#### tests/unary.fur
```
#import "../tests/tests/unary.fur"
```

#### irgen_test.cpp
Contains unit tests for the IR generation and the integrations tests for the whole system
```
#import "../tests/irgen_test.cpp"
```

#### lexer_test.cpp
Unit tests for the lexer
```
#import "../tests/lexer_test.cpp"
```

#### parser_test.cpp
Unit tests for the parser
```
#import "../tests/parser_test.cpp"
```

#### pool_test.cpp
Unit tests for the pool
```
#import "../tests/pool_test.cpp"
```

#### queue_test.cpp
Unit tests for the queue
```
#import "../tests/queue_test.cpp"
```

#### string_test.cpp
Unit tests for the string
```
#import "../tests/string_test.cpp"
```

#### test.cpp
Test program entry point
```
#import "../tests/test.cpp"
```

## References
1. The Rust Reference <a id="1">https://doc.rust-lang.org/reference.html</a>
2. The Go Programming Language Specification <a id="2">https://golang.org/ref/spec</a>
3. Specification for the D Programming Language <a id="3">https://dlang.org/spec/spec.html</a>
4. The F# Language Specification <a id="4">http://fsharp.org/specs/language-spec/</a>
5. Go's GitHub Repository <a id="5">https://github.com/golang/go</a>
6. Go - Proposal: Eliminate STW stack re-scanning <a id="6">https://golang.org/design/17503-eliminate-rescan</a>
7. Plumber - G1 vs CMS vs Parallel GC <a id="7">https://plumbr.eu/blog/garbage-collection/g1-vs-cms-vs-parallel-gc</a>
