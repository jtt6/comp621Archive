#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "csapp.h"

int main(int argc, char *argv[]) {
	int fd_to, fd_from;
	char *buf;
	ssize_t nread;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <source-file> <dest-file>\n",
			argv[0]);
		exit(1);
	}

	fd_from = open(argv[1], O_RDONLY);

	buf = Malloc(sysconf(_SC_PAGE_SIZE));
	fd_to = Open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

	//while (nread = Read(fd_from, buf, sizeof buf), nread > 0) {
	while (nread = rio_readn(fd_from, buf, sizeof buf), nread > 0 ) {
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			//nwritten = Write(fd_to, out_ptr, nread);
			nwritten = rio_writen(fd_to, out_ptr, nread);
			nread -= nwritten;
			out_ptr += nwritten;
		} while (nread > 0);
	}

	Close(fd_to);
	Close(fd_from);
	Free(buf);

	/* Success! */
	return 0;
}
