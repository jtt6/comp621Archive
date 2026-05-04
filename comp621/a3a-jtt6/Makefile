CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -I./include -g -O2
LDFLAGS = -lpthread

all: chat-switchboard chat-switchboard-bcast

chat-switchboard: chat-switchboard.c csapp.o
	${CC} ${CFLAGS} -o chat-switchboard chat-switchboard.c csapp.o ${LDFLAGS}

chat-switchboard-bcast: chat-switchboard-bcast.c csapp.o
	${CC} ${CFLAGS} -o chat-switchboard-bcast chat-switchboard-bcast.c csapp.o ${LDFLAGS}

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	${RM} *.o chat-switchboard chat-switchboard-bcast core.[1-9]*

.PHONY: all clean
