#include <stdio.h>
#include <ctype.h>
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
	int c;

	while ((c = getchar()) != EOF) {
		if (isupper(c)) {
			c = tolower(c);
		}
		if (isspace(c)) {
			c = '-';
		}
		putchar(c);
	}
	/* Returning zero says, "No errors occurred." */
	return (0);
}
