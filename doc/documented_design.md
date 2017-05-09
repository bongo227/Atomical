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

```markdown
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

```c
#import "../src/includes/string.h"
```

#### string.c
This is the source file for the string implementation

```c
#import "../src/string.c"
```

### Pool
A compiler often allocates and frees many of the same structures. You could use the standard memory allocation function `malloc()` and `free()` however these are slow for many small allocations and the data has bad locality, so more cache misses occur when the data is read. The solution to this problem is a custom allocator, in this case a pool allocator.

A pool allocator takes a large chunk of memory and divides it into equal parts. When ever a piece of memory is required, the pool allocator returns a section of this memory. When the memory is freed the pool allocator reclaims this memory ready to give out on the next request.

#### Constructing a pool
The pool must first allocate a large chunk of memory to form the pool. From their the free list is set up by looping through each slice of memory and adding a header with pointer to the next slice of memory. 

```markdown
lastElement = memory[0]
FOR i = 0 TO elementCount
    element <- memory[i]
    lastElement.next <- element
    lastElement <- element
LOOP
lastElement.next <- _
```

Finally we create a structure to save some details about the pool.
```markdown
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

```markdown
FUNCTION poolFull(pool) 
    return pool.head = _
ENDFUNCTION
```

#### Pool count
For debugging purposes its often useful to know how much elements are in the pool. To compute this, we walk the free list to find out how much slices are free, then subtract that from `elementCount`

```markdown
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
```markdown
oldCount <- pool.count
isFull <- poolFull(pool)
```
Then we compute the pointer offset of the head and tail so that we can restore them later.
```markdown
oldMemory <- pool.memory
headOffset <- pool.head - oldMemory
tailOffset <- pool.tail - oldMemory
```
Now we can do the reallocation and restore the head and tail pointers.
```markdown
pool.memory <- realloc(pool.memory, pool.elementSize * pool.elementCount)
pool.head <- pool.memory + headOffset
pool.tail <- pool.memory + tailOffset
```
The free list pointers are also invalidated by the reallocation so they too must be restored.
```markdown
IF !isFull THEN
    ptr <- pool.head
    WHILE ptr != pool.tail
        ptr.next <- pool.memory + ptr.next - oldMemory
        ptr <- ptr.next
    ENDWHILE
ENDIF
```
Now that the pools capacity has expaned we need to expand the free list. First we set up a free list of all the new elements
```markdown
firstNew <- pool.memory[oldCount]
lastElement <- firstNew
FOR i = oldCount TO pool.elementCount
    element <- pool.memory[i]
    lastElement.next = element
    lastElement = element
LOOP
```
The two lists can now be joined unless the pool was full before, then their was not old list so the new list becomes the free list.
```markdown
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

```markdown
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
```markdown
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
```c
#import "../src/includes/pool.h"
```

#### pool.c
This is the source file for the pool implementation
```c
#import "../src/pool.c"
```

### Queue
Queues are an important are another important data structure which the C standard library doesn't provide, so I implemented my own. This queue uses a double ended linked list on the back end allowing for fast insertions and deletions from either end of the queue. This is not strictly a queue, and can also be used as a stack.

#### Queue push
To push onto a queue, the queue allocates a new element with a header which is returned to the caller for populating with data. To push to the front we add the new element at the start of the queue and link the new element and old element.

```markdown
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

```markdown
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
```markdown
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

```markdown
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

```markdown
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

```c
#import "../src/includes/queue.h"
```

#### queue.c
This is the source file for the queue implementation

```c
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

```markdown
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

```markdown
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

```markdown
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
```markdown
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

```c
#import "../src/includes/lexer.h"
```

#### lexer.c
This is the source file for the lexer implementation

```c
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

```markdown
FUNCTION nud(token)
	SWITCH typeof(token)
		# Case statements here ...
	END SWITCH
END FUNCTION
```
###### Literal expression
If a literal is at the beginning of an expression (or sub-expression) we return a literal node, any following operators will be handled in an infix context. This is a key distinction between some other algorithms, by delaying infix operators we are able to parse mathematical expressions like a + b * c without having to use something like the shunting yard algorithm which requires allocating a stack (two stacks for function calls) and queue.

```markdown
CASE INT, FLOAT, HEX, OCTAL, STRING:
	return LiteralNode { token }
```

###### Ident expression
```markdown
CASE IDENT:
	return IdentNode { token }
```

###### Unary expression
A unary node is normally something like `-14`, the operator in front changes the value to the right. To parse a unary node we treat the tokens after the operator as a sub-expression by calling `expression` (see below). 

We parse in a value of 60 for the binding power of the sub expression, this is the other unique feature of a Pratt parser. To delay the parsing of infix tokens we use the concept of binding power, which is how well tokens should _bind_ to each other. For example the binding power of `*` will be higher than the binding power of `-` since we always multiply before subtracting. By calling `expression` we get the next sub expression where all the tokens are bound by a binding power higher than the right binding power (which is the parameter to expression).

`expression` is what gives the ast its tree structure and allows us to parse expressions such as `-(14 + a)`, in this case the expression on the right wasnt a simple literal value but a binary expression.

```markdown
CASE NOT, SUB:
	return UnaryNode { token, expression(60) }
```

###### Key value expression
A key value node is a list of values with keys such as:
```markdown
{
    baz: 123,
    qux: 2.4,
}
```
but the keys may be ommited 
```markdown
{ 123, 2.4 }
```
To parse a list of values we use a loop to parse each key value pair until we reach a `}` which is the end of list. Inside the loop we try to parse a key or a value, since we do not know if the item has a key we must check if the next token is a `:`. If it is then the `keyOrValue` was a key and the value expression is parsed, if their was no colon then `keyOrValue` must be a value. A `KeyValue` node is then appended to the `keyValues` array which will form the `KeyValueList` node. 

```markdown
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

```markdown
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
```markdown
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

```markdown
CASE ADD, SUB, MUL, QUO, REM, 
    EQL, NEQ, GTR, LSS, GEQ, LEQ, LAND, LOR,
    ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
    
    RETURN BinaryExp{exp, token, expression(rbp)}
```

###### Selector expression
A selector expression is exactly the same as a binary expression except the two values are separated by a `.`.

```markdown
CASE PERIOD:
    RETURN SelectorExp{exp, expression(rbp)}
```

###### Call expression
A call expression is similar to a key value list expression.

```markdown
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
```markdown
CASE RETURN:
    RETURN Return{expression()}
```

###### Block statement
Block statements are used in more complex statements such as if and for statments or on their own.
```markdown
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

```markdown
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

```markdown
CASE FOR:
    expect(FOR)
    index <- declaration()
    condition <- expression()
    body <- block()
```

###### Variable declaration
Variable declarations are normally handled elsewhere however if it starts with the optional `var` then we can parse the variable declaration now (rather than later).

```markdown
CASE VAR:
    RETURN declaration()
```

###### Increment expression
Increment statements do not start with a keyword, so we look for an identifier. If their is not a proceeding increment or decrement token then we know the expression is a assignment or declaration node so we let the caller handle it.

```markdown
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

```markdown
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

```markdown
CASE VAR:
    type <- Type()
    name <- Ident()
    value <- Expression()
    RETURN VariableDcl{type, name, value}
```

Otherwise the statement wrapper function first try's to parse it as a statement with `smtd()`, for short variable declarations this will return `NULL`. Next it try's parsing it as an expression, this will return a binary expression node where the operator is a assign or define type. From their the expression is converted into a statement.

```markdown
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

```markdown
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

```markdown
FUNCTION skipSemi()
    WHILE token != SEMI THEN
        nextToken()
    ENDWHILE
    nextToken()
ENDFUNCTION
```

#### parser.h
This is the header file for the parser implementation
```c
#import "../src/includes/parser.h"
```

#### parser.c
This is the source file for the parser implementation
```c
#import "../src/parser.c"
```

### IR Generation
Having converted the source code to tokens and the tokens to an abstract syntax tree we now can transform it into LLVM IR which LLVM will compile to machine code. LLVM IR is much lower level than Fur however the LLVM C API does a lot of the heavy lifting for by constructing the intermediary language in memory.

#### If statements
LLVM IR does not have any if statements only conditional branches. To branch we must break the code into separate blocks which end with a terminating statements (such as a branch, return etc). Since if statements can be chained it make sense for the transformation to be recursive hence we need a two functions, the recursive `compileIfBranch()` function and the `compileIf()` function to call into the recursive function.

`compileIf()` just sets up a `endBlock` which is the block which the program will continue executing from.
```markdown
FUNCTION compileIf(irgen, ifSmt)
    endBlock <- LLVMAppendBasicBlock(irgen.function, "endBlock")
    compileIfBranch(irgen, ifSmt, _, endBlock)
ENDFUNCTION
```

`compileIfBranch()` takes four arguments: a reference to irgen which holds some state about the ir generation, the if node, the block to branch from (will be `NULL` for root level if branches) and the block to continue execution from. 

The first step is to check for a condition, if the condition is `NULL` then this branch must be an else branch therefore we compile the block and exit. `CompileBlockAt()` compiles a block statement and returns an `LLVMBlockRef` to were the execution should continue. We use `SetBlock()` to set it as are current block and check if its not terminated, if its not then we insert a branch to the end block.
```markdown
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
```markdown
parent <- _
IF block = _ THEN
    parent <- irgen.block
ELSE
    parent <- block
ENDIF
```

`block` now becomes the block if the condition is true
```markdown
block <- LLVMAppendBasicBlock(irgen.function, "if")
```

`falseBlock` will be either the next else/else if branch or the block to continue execution
```markdown
falseBlock <- _
IF ifSmt.else != _ THEN
    falseBlock <- LLVMAppendBasicBlock(irgen.function, "else")
ELSE
    falseBlock <- endBlock
ENDIF
```

Now we can compile the body of the current if/else if branch, if the execution is not terminated we also branch to `endBlock`
```markdown
outBlock <- CompileBlockAt(irgen, ifSmt.body, block)
IF LLVMGetBasicBlockTerminator(outBlock) = _ THEN
    SetBlock(irgen, outBlock)
    LLVMBuildBr(irgen.builder, endBlock)
    SetBlock(irgen, parent)
ENDIF
```

The conditional branch instruction is next.
```markdown
condition <- CompileExp(irgen, cond);
LLVMBuildCondBr(irgen.builder, condition, block, falseBlock);
```

Next we check for any chaining else if/else blocks. If their are we recursively call this function to compile the whole chain.
```markdown
SetBlock(irgen, falseBlock)
IF ifSmt.else != _ THEN
    CompileIfBranch(irgen, ifSmt.else, falseBlock, endBlock)
ENDIF
```

Finally we set the end block as the current block so any further instructions execute from their.
```markdown
SetBlock(irgen, endBlock)
```

#### irgen.h
This is the header file for the IR generation implementation
```c
#import "../src/includes/irgen.h"
```

#### irgen.c
This is the source file for the IR generation implementation
```c
#import "../src/irgen.c"
```