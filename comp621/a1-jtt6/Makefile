CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g
SORT = ./sort_select_buggy
PROGRAMS = ./program1 ./program2

.PHONY: all clean

$(SORT):
	$(CC) $(CFLAGS) $(SORT).c -o $(SORT)

all: $(SORT) $(PROGRAMS)

program1: program1.c
	$(CC) $(CFLAGS) program1.c -o program1

program2: program2.c
	$(CC) $(CFLAGS) program2.c -o program2

clean:
	$(RM) *.o $(SORT) $(PROGRAMS) core.[1-9]*
