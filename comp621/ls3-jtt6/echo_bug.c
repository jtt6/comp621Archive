#include <stdio.h>

/*
 * Requires:
 *   Nothing.
 *
 * Effects:
 *   Echo stdin to stdout.
 */
int
main(void)
{
	char c;

	while ((c = getchar()) != EOF)
		putchar(c);
	/* Returning zero says, "No errors occurred." */
	return (0);
}
