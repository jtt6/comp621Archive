#include <stdio.h>

/* Put your functions here. */

int
main(void)
{
	int n = 5;

	printf("Factorial of %d is %d = %d.\n",
	    n, fact_for(n), fact_while(n));
	return (0);
}
