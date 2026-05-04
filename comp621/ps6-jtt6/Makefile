CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lnsl -lrt

gc: gc.o test.o
	$(CC) $(CFLAGS) test.o gc.o -o gc $(LDFLAGS)

gc.o: gc.c gc.h
	$(CC) $(CFLAGS) gc.c -c
 
test.o: test.c test.h
	$(CC) $(CFLAGS) test.c -c

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	$(RM) *.o gc core.[1-9]*

.PHONY: clean
