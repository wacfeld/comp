all: main.c
	cc main.c datastruct.c -o main -g -pg -fsanitize=undefined -fsanitize=address
warn: main.c
	cc main.c datastruct.c -o main -g -pg -Wextra -Wall -fsanitize=undefined -fsanitize=address
test: test.c
	gcc test.c -o test -ansi -pedantic -Wextra -Wall -fsanitize=undefined -fsanitize=address
disas:
	gcc -S test.c -ansi -pedantic -Wextra -Wall -fsanitize=undefined -fsanitize=address
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
