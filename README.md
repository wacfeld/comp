# Description
A C compiler written that I wrote between March and June 2022. It was written entirely from scratch in C, and compiles to x86 assembly. I spent approximately 100-200 hours on this project.

# usage
```
touch main.c
# put code in main.c
./comp main.c main
./main
```

# Code overview
- `src/main.c`:
  - If you use vim, you can `:set foldmethod=marker` to make navigating this file easier. Otherwise, you can simply search for `{{{1` headings
  - `debugging` contains debugging macros/functions
  - `global mutables` are state variables which are used by the parser and code generator (scope, declarations, stack frame size, string literals)
  - `lexer` contains functions which recognize tokens, as well as detect simple syntax errors (e.x. stray backtick, unmatched quotes, etc.). Ultimately it turns the source code into a list of tokens. This could have been done more efficiently with regex, but I did it with many if statements
  - `tools` contains parts of the the declaration/prototype parser and type checking apparatus, as well as some other miscellaneous functions
  - `decls` contains the declaration parser and initializer parser
  - `linked list tools` contains tools for dealing with linked lists
  - `expr manipulation` contains functions which create and operate on expressions. It also implements array decay to pointer, function decay to pointer, lvalue decay to value, etc.
  - `type checking tools` contains functions which operate on types (check compatibility, compare type qualifiers, detect illegal types, casting, perform usual arithmetic conversions/integral promotion, etc.)
  - `expression parser` parses expressions, according to the precedence rules specified by the C standard. This is not entirely trivial as several operators in C (such as multiplication and dereferencing) are represented by the same tokens. This took a while to figure out and a lot of testing.
    - A separate function exists for each level of precedence. Lowest precedence operators are parsed first, thus, when a `*` is encountered, it is first interpreted as multiplication, not dereferencing. If a higher precedence function encounters an illegal expression, it signals this to its caller, which attempts to reinterpret the given expression. This happens until the correct interpretation for the expression is found, or, if the expression is simply invalid, the compiler exits with an error
    - For example, `b * sizeof * a` would first by interpreted by `parsemultexpr` as `((b) * (sizeof)) * (a)`, two multiplications. It would then call `parsecastexpr` on `b`, `sizeof`, and `a`. Eventually some function will realize that `sizeof` on its own is not a valid expression, so it would return control to `parsemultexpr`, which would decide to reinterpret the second `*` as not a multiplication. Eventually the expression would be correctly interpreted as `b * (sizeof (*a))`
  - `constant expressions` takes integral constant expressions and evaluates them at compile time, for use in array size definitions and global initializers.
  - `string operations` contains functions for the code generator. The code generator works by maintaining a separate string for the data segment, code segment, and bss segment. These functions allow the code generator to append to them, and also provide several idiomatic assembly language constructs (stack allocation/deallocation, memory addressing, etc.)
  - `toplevel` contains functions for parsing top level declarations, as well as invoking the expression parser and code generator on function bodies
  - `scope operations` maintains symbol table (keeping track of which declarations are in scope), which is implemented as a stack
    - Declarations (which consist of type, identifier, global or local, location, etc.) are pushed onto the stack as they appear (this includes top level declarations)
    - Whenever a `{ ... }` block is encountered, a 'null' is pushed onto the stack, which allows for identifier shadowing. Two identical identifiers are allowed to coexist if there is a 'null' between them
    - Identifiers are matched with memory addresses by looking through the stack from top to bottom, finding the first match
    - When a block is exited, everything down to the latest 'null' is popped, removing the local variables from scope
    - `statement eval` is the statement parser and code generator. Instead of creating an intermediate data format for statements, I simply generated code for them immediately. This contrasts with the expression parser, which produces `expr` structs before running the code generator on them
    - `expr eval` is the expression code generator. It contains functions that take in `expr` structs and produce the code for evaluating them. These functions are called by the statement parser, which detects expressions and calls the expression parser on them. All evaluation is done on the stack (and all local variables/parameters are stored on the stack)
    - `main` contains the main function, which:
      - Splices lines with backslashes
      - Checks for stray illegal characters
      - Calls the lexer, creating an array of tokens
      - Calls `proctoplevel`, which processes top level declarations and generates code for the function bodies, and prints the resulting code to stdout
- `src/defs.h`:
  - Contains debugging macros and strings
  - Defines constants (`INT_SIZE`, `INT_MAX`, etc.)
  - Contains enums and structs for the compiler to use (tokens, expressions, declarations, operations, expressions, etc.)
  - It is not technically a header file, as it defines variables. It is just included at the top of `main.c`
- `src/datastruct.c` and `datastruct.h` define structs and functions for sets, stacks, and queues, which are used by the compiler
- `src/preproc.c` contains a limited preprocessor which only removes comments
- `src/stdio.asm` contains defines the standard library functions `exit()`, `putchar()`, and `getchar()`, written in assembly
- `compile`, `assemble`, and `link` are shell scripts which call the compiler (written by me), assembler (NASM), and linker (ld) respectively. `comp` simply calls these 3 scripts in order.
- `build/main` the compiler executable
- `build/preproc` the preprocessor executable
- `build/stdio.o` the standard library object file

# system
- all integers are 4 bytes. pointers are 4 bytes.
- runs on x86 32-bit mode
- compiles to netwide assembly language (NASM)

# dependencies
## nasm
```sudo apt-get install nasm```

## gcc
```sudo apt-get install gcc```

# build
```
make
```

# unsupported features
- floating point
- enums
- structs, unions
- typedef
- extern, static, register, auto keywords
- variadic functions ('...')
- old-style function declarations
- command line arguments (argc, argv)
- wide characters
- multiple files (only one file with main() in it)
- implicit function declaration (calling a function before it's declared)
- preprocessor directives (`#define`, `#include`, `#if`, etc.)
- string literal concatenation ("hello " "world" -> "hello world")
- anything introduced after C90 (variable length arrays, long long int, etc.)
- compound assignment expressions (a += b, a *= b)
- switch/case/default statements
- array initializers

# partially supported features
- string literals are treated as pointers to characters. therefore sizeof("any string") is always 4 (the size of a pointer) and strings should be declared as 'char *s = ...' instead of 'char s[] = ...'
- `int getchar(void)`, `void putchar(int)`, `void exit(void)` are builtin. the rest of the standard library is not supported. 
  - however, you have to declare these functions before using them (see example/example.c)
- constant integral expressions are supported but may be buggy

