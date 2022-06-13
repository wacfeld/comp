# unsupported features (incomplete)
- floating point
- comments
- enums
- structs, unions
- typedef
- extern, static, register, auto keywords
- complex constant expressions (3+4 is not allowed, 3 is allowed)
- variadic functions ('...')
- old-style function declarations
- the standard library, except for getchar(), putchar(), exit()
- command line arguments (argc, argv)
- wide characters
- multiple files (only one file with main() in it)
- implicit function declaration (calling a function before it's declared)
- `#define`, `#include`, `#if`, etc.
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
mkdir build
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
