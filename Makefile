all: main.c
	cc main.c datastruct.c parser.c -o main -g
test: test.c
	gcc test.c -o test -ansi -pedantic -Wextra -Wall
disas:
	gcc -S test.c -ansi -pedantic -Wextra -Wall
in:
	make all
	./main < in.txt
debug:
	make all
	gdb main
run:
	make all
	./main
clean:
	rm -rf main
