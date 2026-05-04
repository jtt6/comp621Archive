#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 1024*128

int main() {
	size_t i;
	long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
	char * addr[N];

	puts("Waiting.  Press Enter to allocate.");
	getchar();

	printf("Allocating and initializing %d pages.\n", N);
	for (i = 0; i < N; i++) {
		addr[i] = (char *) malloc(PAGE_SIZE);
		memset(addr[i], 'a', PAGE_SIZE);
	}

	puts("Allocated.  Press Enter to deallocate.");
	getchar();

	printf("Deallocating every other page.\n");
	for (i = 0; i < N; i += 2) {
		free(addr[i]);
	}

	puts("Deallocated.  Press Enter to quit.");
	getchar();
	return 0;
}
