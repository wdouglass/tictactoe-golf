
CC = musl-gcc
CFLAGS=--std=c99 -Os -s
LDFLAGS=
all: tictactoe tictactoe-static

tictactoe: tictactoe.o
	${CC} -o $@ $^ ${LDFLAGS}
	strip $@

tictactoe-static: tictactoe.o
	${CC} -o $@ $^ ${LDFLAGS} -static
	strip -S --strip-unneeded $@
