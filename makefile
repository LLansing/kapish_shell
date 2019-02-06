CFLAGS=-Wall -Werror -pedantic-errors -std=c11
CC=gcc
DEPS = utilfunctions.h
kapish: kapish.o utilfunctions.c
	gcc kapish.o utilfunctions.c -o kapish $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kapish.o: kapish.c
	$(CC) -c kapish.c -o kapish.o $(CFLAGS)
