#include <sys/mman.h>
#include <sys/stat.h> 
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * This NUL-terminated string is the data that should be written to the shared
 * buffer by the child.  Once the child has done this (and returned), the
 * parent should check to see if the child successfully wrote the data to the
 * buffer.  You might find wait() or waitpid() useful.
 */
const char string[] = "Some data that will be shared between the child and "
    "parent processes";

/*
 * Requires:
 *   None.
 *
 * Effects:
 *   Demonstrates how mmap() can be used to share memory across processes.
 */
int
main(void)
{
	int pid;

        // Allocate space.
	// char *buf = calloc(strlen(string) + 1, sizeof(char));
	char *buf = mmap(NULL, strlen(string) + 1, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (buf == NULL) {
	        perror("calloc");
		return (1);
	}
	
	if ((pid = fork()) == 0) {
		// This is the child process.
		
		// Write the data to the buffer.
		strcpy(buf, string);
	} else if (pid > 0) {
		// This is the original process, the parent.
		
		// Make sure this runs second, so that the strcopy() happens first.
		// Using sleep() is a hack, but simple.
		sleep(5);
		
		// Make sure the data are equal.
		if (strcmp(buf, string) != 0) {
			fprintf(stderr,
				"Wrong string!  Got \"%s\", expected \"%s\".\n",
				buf, string);
		} else {
			printf("Everything worked!\n");
		}
	} else {
	        perror("fork");
		return (1);
	}
	return (0);
}
