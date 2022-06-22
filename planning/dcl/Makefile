all: main.c
	cc main.c -o main -g
debug:
	make all
	gdb main
run:
	make all
	./main
clean:
	rm -rf main
