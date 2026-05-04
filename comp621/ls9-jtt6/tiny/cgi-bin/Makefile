CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -I../../include -g

all: adder

adder: adder.c
	${CC} ${CFLAGS} -o adder adder.c

clean:
	${RM} adder core.[1-9]*

.PHONY: all clean
