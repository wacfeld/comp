build: src/main.c src/datastruct.c src/preproc.c
	mkdir -p build
	cc src/main.c src/datastruct.c -o build/main -g -pg
	cc src/preproc.c -o build/preproc
	nasm -f elf -g -F dwarf src/stdio.asm -l build/stdio.lst -o build/stdio.o
san:
	mkdir -p build
	cc src/main.c src/datastruct.c -o build/main -g -pg -fsanitize=undefined
	cc src/preproc.c -o build/preproc -fsanitize=undefined
	nasm -f elf -g -F dwarf src/stdio.asm -l build/stdio.lst -o build/stdio.o
