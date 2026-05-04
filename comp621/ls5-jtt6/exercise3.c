#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * Requires:
 *   A source and destination file must be specified on the command line.
 *   The source file must exist.
 *
 * Effects:
 *   Performs a fast file copy using mmap() to read from the source file and
 *   write to the destination file.  If the destination file does not exist,
 *   it is created.  The size of the destination file is set to the size of
 *   the source file.
 */
int
main(int argc, char **argv)
{
	struct stat stat;
	int destfd, size, srcfd;
	char *destbuf, *srcbuf;

	/*
	 * WARNING: Before you're confident in your implementation, be careful
	 * in choosing the source and destination files that you use to test
	 * this program.  You could, for instance, accidentally overwrite one
	 * of your exercise.c files, if you use one of them as an argument to
	 * this program.  At first, test with small, dummy text files.
	 */

	// Check the arguments passed in.
	if (argc != 3) {
		printf("Usage: %s <source file> <destination file>\n",
		    argv[0]);
		return (1);
	}

	// The following is just like exercise1.c.
	
	// Open the source file.
	srcfd = open(argv[1], O_RDONLY);
	if (srcfd < 0) {
	        perror("open");
	        return (1);
	}

	// Get the source file's size.
	if (fstat(srcfd, &stat) < 0) {
	        perror("fstat");
		return (1);
	}
	size = stat.st_size;

	// Map the source file to a new virtual memory area.
	srcbuf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, srcfd, 0);
	if (srcbuf == MAP_FAILED) {
	        perror("mmap");
		return (1);
	}
	

	
	// Dummy statements to prevent compilation warnings.
	// Remove these when you complete the following section.
	destfd = 0;
	destbuf = NULL;
	
	// Open the destination file and get its size.
	// CHANGE THIS.
	// * Need to be able to read and write file.
	// * Need to be able to truncate file.
	// * Need to create it with the appropriate permissions if it
	//   doesn't exist.
	destfd = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
	if (destfd < 0) {
	        perror("destination open");
		return (1);
	}

	// Truncate the destination file to the correct size.
	// Needs to be done after the fd
	// is open, but before it is used (in mmap).
	// COMPLETE THIS.
	ftruncate(destfd, size);
	// Map the destination file to a new virtual memory area.
	// CHANGE THIS.
	// * Need to be able to write buffer to file.
	destbuf = mmap(NULL, size, PROT_WRITE, MAP_SHARED, destfd, 0);
	if (destbuf == MAP_FAILED) {
	        perror("destination mmap");
		return (1);
	}
	
	// Copy the data from the source buffer to the destination buffer.
	// COMPLETE THIS.
	memcpy(destbuf, srcbuf, size);
	// Clean up.  Ignore the return values because we exit anyway.
	(void)munmap(srcbuf, size);
	(void)close(srcfd);
	(void)munmap(destbuf, size);
	(void)close(destfd);

	return (0);
}
