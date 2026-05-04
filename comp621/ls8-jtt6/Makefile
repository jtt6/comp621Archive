CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lnsl -lrt

PROGS = echoclient echoserver

all: ${PROGS}

echoclient: echoclient.o csapp.o
	${CC} ${CFLAGS} -o echoclient echoclient.o csapp.o ${LDFLAGS}

echoserver: echoserver.o echo.o csapp.o
	${CC} ${CFLAGS} -o echoserver echoserver.o echo.o csapp.o ${LDFLAGS}

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	${RM} *.o ${PROGS} core.[1-9]*

.PHONY: clean
