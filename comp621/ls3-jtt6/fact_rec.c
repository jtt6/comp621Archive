#include <stdio.h>

int fact_rec(int n);

/* Put your function here. */
int
fact_rec(int n)
{
	if (n <= 1) {
		return (1);
	} else { 
		return n * fact_rec(n - 1);
	}
}

int
main(void)
{
	int n = 5;

	printf("Factorial of %d is %d.\n",
	    n, fact_rec(n));
	return (0);
}
