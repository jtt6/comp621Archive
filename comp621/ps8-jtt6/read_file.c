#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/csapp.h"
#define BUF_SIZE 4096

#define N 10

void read_file(int n) {
	char* buf;
	int i, fd;
	clock_t start, end;
	
	buf = (char*)malloc(BUF_SIZE);
	if(!buf)
		exit(-1);
	fd = Open("poem.txt", O_RDONLY, 0);

	start = clock();
	
	for (i = 0; i < n; i++) {
		//posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
		//posix_fadvise(fd, 0, 0, POSIX_FADV_NORMAL);
		//posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM);
		//posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
		Lseek(fd, 0, SEEK_SET);
		while(Read(fd, buf, BUF_SIZE) > 0);
	}
	end = clock();
	printf("%f\n", ((double)(end - start)) / CLOCKS_PER_SEC);
	free(buf);
	Close(fd);
}

int main() {
	read_file(N);
}
