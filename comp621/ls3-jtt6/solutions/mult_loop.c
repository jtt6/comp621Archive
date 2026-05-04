/*
 * Sample solution for a beginning C exercise to write factorial function.
 */

#include <stdio.h>

/*
 * Return the factorial of n.
 * Computes the factorial recursively.
 */
int
fact_rec(int n)
{

	if (n == 0)
		return (1);
	else
		return (n * fact_rec(n - 1));
}

int
main(void)
{
	int n = 5;

	printf("Factorial of %d is %d.\n", n, fact_rec(n));
	return (0);
}
