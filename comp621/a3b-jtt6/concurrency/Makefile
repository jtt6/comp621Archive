CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lnsl -lrt
PROGS = race deadlock livelock starvation

all: $(PROGS)

race: race.c csapp.o
	$(CC) $(CFLAGS) -o race race.c csapp.o $(LDFLAGS)

deadlock: deadlock.c csapp.o
	$(CC) $(CFLAGS) -o deadlock deadlock.c csapp.o $(LDFLAGS)

livelock: livelock.c csapp.o
	$(CC) $(CFLAGS) -o livelock livelock.c csapp.o $(LDFLAGS)

starvation: starvation.c csapp.o
	$(CC) $(CFLAGS) -o starvation starvation.c csapp.o $(LDFLAGS)

csapp.o: ./include/csapp.c ./include/csapp.h
	${CC} ${CFLAGS} -c ./include/csapp.c -o csapp.o

clean:
	$(RM) *.o $(PROGS) core.[1-9]*

.PHONY: all clean
