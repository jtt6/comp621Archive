CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lrt -lm

cachesim: cachesim.o cache.o csapp.o
	$(CC) $(CFLAGS) -o cachesim cachesim.o cache.o csapp.o $(LDFLAGS)

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	$(RM) *.o cachesim core.[1-9]*

.PHONY: clean
