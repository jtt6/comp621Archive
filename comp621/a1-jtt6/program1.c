#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
* Requires:
* 	Nothing
*
* Effects:
* 	Creates an array of 10000 elements
*	of values ranging from 0 to 99.
*/
int*
create_array(void){
	int *arr = (int*)malloc(10000 * sizeof(int));
	if (arr == NULL)
		return NULL;

	for (int j = 0; j < 10000; j++) {
		arr[j] = rand() % 100;
	}

	return arr;
}

/*
* Requires:
*	- A pointed to an array of ints
*
*
* Effects:
*	Sums the elements of an array
*/
long long
sum_array(int *arr) {
	long long sum = 0;

	for (int i = 0; i < 100000; i++) {
		for (int j = 0; j < 10000; j++) {
			if (arr[j] >= 50)
				sum += arr[j];
		}
	}

	return sum;
}

/*
* Requires:
*	Nothing
*
* Effects:
*	Add array elements that satisfy
*	a[i] >= 50.
*/
int
main(void) {
	// Random seed generator
	srand((unsigned) time(NULL));

	// Create array
	int *a = create_array();
	if (a == NULL)
		return (1);

	// Sum array
	long long sum = sum_array(a);

	printf("Final sum = %lld\n", sum);
	free(a);
	return (0);
}
