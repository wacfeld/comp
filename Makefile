all: main.c
	cc main.c defs.h -o main -g
test: test.c
	gcc test.c -o test -ansi -pedantic -Wextra -Wall
disas:
	gcc -S test.c -ansi -pedantic -Wextra -Wall
debug:
	make all
	gdb main
run:
	make all
	./main
clean:
	rm -rf main
