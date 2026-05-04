/*
 * Sample solution for a beginning C exercise to write multiply function.
 */

#include <stdio.h>

/*
 * Return m*n.
 * Computes the product recursively.
 */
int
mult_rec(int m, int n)
{

	if (n == 0)
		return (0);
	else
		return (m + mult_rec(m, n - 1));
}

int
main(void)
{
	int m = 3;
	int n = 5;

	printf("%d * %d is %d.\n", m, n, mult_rec(m, n));
	return (0);
}
