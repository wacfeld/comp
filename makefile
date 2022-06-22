build: src/main.c src/datastruct.c src/preproc.c
	mkdir -p build
	cc src/main.c src/datastruct.c -o build/main -g -pg
	cc src/preproc.c -o build/preproc
	nasm -f elf -g -F dwarf src/stdio.asm -l stdio.lst -o build/stdio.o
