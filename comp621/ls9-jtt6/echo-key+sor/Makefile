CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -I../include -g -O2
LDFLAGS = -lpthread

PROGS = echo-key sor-pthreads

all: ${PROGS}

echo-key: echo-key.c csapp.o
	${CC} ${CFLAGS} -o echo-key echo-key.c csapp.o ${LDFLAGS}

sor-pthreads: sor-pthreads.c csapp.o
	${CC} ${CFLAGS} -o sor-pthreads sor-pthreads.c csapp.o ${LDFLAGS}

csapp.o: ../include/csapp.c ../include/csapp.h
	${CC} ${CFLAGS} -c ../include/csapp.c -o csapp.o

clean:
	${RM} *.o ${PROGS} core.[1-9]*

.PHONY: all clean
