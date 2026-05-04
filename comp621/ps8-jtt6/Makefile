CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lnsl -lrt

read_file: read_file.c csapp.o
	$(CC) $(CFLAGS) -o read_file read_file.c csapp.o $(LDFLAGS)

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	$(RM) *.o read_file core.[1-9]*

.PHONY: clean
