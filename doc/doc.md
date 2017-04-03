# Furlang: An investigation into modern programming languages and compilers

## Analysis

In this investigation the aim is to design a programming language and implement the basics of a compiler to create executable programs. Due to the time constraints it would be infeasible to implement all asspects of a modern programming language, standard library and language tooling. Instead the focus wull be on implementing a small subset such that simple algorithums like the greatest common divisor and bubble sort can be created.

### Research
In terms of the languages design I looked at several languages with similar goals as mine and read through their specifications including: [Rust<sup>[1]</sup>](#1), [Go<sup>[2]</sup>](#2), [D<sup>[3]</sup>](#3) and [F#<sup>[4]</sup>](#4). I looked at their syntax and the design disicions behind them in order the judge what Fur should look like.

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

#### Objectives
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

### Irgen.c
This file is the ir generation...

```c
#import "../src/irgen.c"
```