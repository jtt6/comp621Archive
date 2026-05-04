all: ipc-static ipc-dynamic
CC=gcc
CFLAGS=-DWITH_PMC -Wall

ipc-static: ipc.c
	${CC} ${CFLAGS} -o $@ ipc.c -static -lpthread

ipc-dynamic: ipc.c
	${CC} ${CFLAGS} -o $@ ipc.c -lpthread
