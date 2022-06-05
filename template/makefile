main: main.o
	ld -o main main.o
main.o: main.asm
	nasm -f elf64 -g -F dwarf main.asm -l main.lst
test: test.o
	ld -o test test.o
test.o: test.asm
	nasm -f elf64 -g -F dwarf test.asm -l test.lst
