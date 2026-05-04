/*
 * Sample solution for a beginning C exercise to write factorial functions.
 */

#include <stdio.h>

/*
 * Return the factorial of n.
 * Computes the factorial iteratively with a for-loop.
 */
int
fact_for(int n)
{
	int result = 1;
	int i;

	for (i = n; i > 0; i -= 1)
		result *= i;
	return (result);
}

/*
 * Return the factorial of n.
 * Computes the factorial iteratively with a while-loop.
 */
int
fact_while(int n)
{
	int result = 1;
	int i = n;

	while (i > 0) {
		result *= i;
		i -= 1;
	}
	return (result);
}

int
main(void)
{
	int n = 5;

	printf("Factorial of %d is %d = %d.\n", n, fact_for(n), fact_while(n));
	return (0);
}
