
CC = gcc
CFLAGS=--std=c99 -Os
LDFLAGS=
all: tictactoe tictactoe-static

tictactoe: tictactoe.o
	${CC} -o $@ $^ ${LDFLAGS}
	strip $@

tictactoe-static: tictactoe.o
	${CC} -o $@ $^ ${LDFLAGS} -static
	strip $@
