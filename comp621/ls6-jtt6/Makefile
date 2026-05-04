CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror -g

fragment: fragment.c
	$(CC) $(CFLAGS) fragment.c -o fragment

clean:
	${RM} *.o fragment core.[1-9]*

.PHONY: clean
