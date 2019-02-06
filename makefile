CFLAGS=-Wall -Werror -pedantic-errors -std=c11
kapish: kapish.o
	gcc kapish.o -o kapish $(CFLAGS)

kapish.o: kapish.c
	gcc -c kapish.c -o kapish.o $(CFLAGS)
