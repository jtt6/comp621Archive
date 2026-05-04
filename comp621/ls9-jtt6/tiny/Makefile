CC = gcc
CFLAGS = -std=gnu11 -Wextra -I../include -g
LDFLAGS = -lpthread

all: tiny cgi

tiny: tiny.c csapp.o
	${CC} ${CFLAGS} -o tiny tiny.c csapp.o ${LDFLAGS}

csapp.o: ../include/csapp.c ../include/csapp.h
	${CC} ${CFLAGS} -c ../include/csapp.c -o csapp.o

cgi:
	(cd cgi-bin; make)

clean:
	${RM} *.o tiny core.[1-9]*
	(cd cgi-bin; make clean)

.PHONY: all cgi clean
