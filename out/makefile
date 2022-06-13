main: main.o
	ld -m elf_i386 -o main main.o
main.o: main.asm
	nasm -f elf -g -F dwarf main.asm -l main.lst
