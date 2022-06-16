# WORK IN PROGRESS

# unsupported features (incomplete)
- floating point
- enums
- structs, unions
- typedef
- extern, static, register, auto keywords
- variadic functions ('...')
- old-style function declarations
- the standard library, except for getchar(), putchar(), exit()
- command line arguments (argc, argv)
- wide characters
- multiple files (only one file with main() in it)
- implicit function declaration (calling a function before it's declared)
- `#define`, `#include`, `#if`, etc.
- string literal concatenation ("hello " "world" -> "hello world")
- anything introduced after C90 (variable length arrays, long long int, etc.)

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
mkdir -p build
gcc src/main.c src/datastruct.c -o build/comp
```

# usage
## compile
type your C code in `main.c`
```
build/comp main.c main.asm
```

## assemble
```
nasm -f elf -g -F dwarf main.asm -l main.lst
ld -m elf_i386 -o main main.o
```

## run
```
./main
```
