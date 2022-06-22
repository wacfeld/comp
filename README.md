# WORK IN PROGRESS

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
- getchar(), putchar(), exit() are builtin. the rest of the standard library is not supported.
- constant integral expressions are supported but may be buggy

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

# usage
## compile
type your C code in `main.c`
```
./comp main.c main
```

## run
```
./main
```
