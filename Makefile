all: main.c datastruct.c
	cc main.c datastruct.c -o main -g -pg -fsanitize=undefined
warn: main.c datastruct.c
	cc main.c datastruct.c -o main -g -pg -Wextra -Wall -fsanitize=undefined
sanit: main.c datastruct.c
	cc main.c datastruct.c -o main -g -pg -Wextra -Wall -fsanitize=undefined -fsanitize=address
test: test.c
	gcc test.c -o test -ansi -pedantic -Wextra -Wall -fsanitize=undefined -fsanitize=address -pedantic-errors
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
