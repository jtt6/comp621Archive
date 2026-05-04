#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

// Define the settings for how to print integers.
struct config {
	int	num_ints;
	bool	print_even;
	bool	print_odd;
	bool	print_sum;
};

static void	print_ints(const struct config *);

/*
 * Requires:
 *   A valid, readable configuration file named "ints.conf" must reside in
 *   the current working directory.
 *
 * Effects:
 *   Prints integers to stdout based on the configuration file "ints.conf".
 *   Local configuration changes within this program should not be reflected
 *   in the configuration file.
 */
int
main(void)
{
	struct config *conf;
	int fd;

	// Open the file and get its size.
	fd = open("ints.conf", O_RDWR);
	if (fd < 0) {
	        perror("open");
		return (1);
	}

	// Map the file to a new virtual memory area.
	conf = mmap(NULL, sizeof(*conf), PROT_READ | PROT_WRITE, MAP_PRIVATE,
		    fd, 0);
	if (conf == MAP_FAILED) {
	        perror("mmap");
		return (1);
	}

	/*
	 * Do some printing.  After you have fixed the bug, change some of the
	 * configuration and rerun the program.  You should observe your new
	 * configuration not being saved to disk, and thus not being the first
	 * run.
	 */
	printf("Initial print...\n");
	print_ints(conf);
	conf->num_ints = 20;
	conf->print_even = false;
	conf->print_sum = true;
	printf("After changing some values...\n");
	print_ints(conf);

	// Clean up.  Ignore the return values because we exit anyway.
	(void)munmap(conf, sizeof(*conf));
	(void)close(fd);

	return (0);
}

/*
 * Requires:
 *   A valid configuration structure.
 *
 * Effects:
 *   Print some integers to stdout based on the provided configuration.
 */
static void
print_ints(const struct config *conf)
{
	long sum;

	printf("Here are some integers: ");
	sum = 0;
	for (int i = 0; i < conf->num_ints; i++) {
		if ((i % 2) != 0) {
			// Handle odd integers.
			if (conf->print_odd) {
				sum += i;
				printf("%d ", i);
			}
		} else {
			// Handle even integers.
			if (conf->print_even) {
				sum += i;
				printf("%d ", i);
			}
		}
	}
	printf("\n");
	if (conf->print_sum) {
		printf("Here is their sum: %ld\n\n", sum);
	} else {
		printf("\n");
	}
}

