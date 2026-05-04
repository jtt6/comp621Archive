CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lrt
PROGS = mmcopy copyfile

all: $(PROGS)

mmcopy: mmcopy.o csapp.o
	$(CC) $(CFLAGS) -o mmcopy mmcopy.o csapp.o $(LDFLAGS)

copyfile: copyfile.o csapp.o
	$(CC) $(CFLAGS) -o copyfile copyfile.o csapp.o $(LDFLAGS)

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	$(RM) *.o $(PROGS) core.[1-9]*

.PHONY: clean
