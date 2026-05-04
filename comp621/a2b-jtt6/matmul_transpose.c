#include <stdlib.h>

#include "csapp.h"
#include <time.h>
#include <stdio.h>
/* Matrix multiplication
   N = size -- assumes all matrices square
   A,B = inputs
   C = output, not assumed initialized

   Note: The matrix C here has a __restrict__ attribute.
   This tells the compiler (GCC) that C does not overlap
   with other pointers (like A and B). This allows the
   compiler to optimize the matrix multiple with
   vectorization.
*/
void matmul(int N, float (*A)[N], float (*B)[N],
	    float (*__restrict__ C)[N]) {
	int i, j, k;
	float(*BT)[N];
	clock_t start_time, end_time;
	double exec_time;
	
	start_time = clock();
	
	BT = Malloc(sizeof(float[N][N]));

	for (i = 0; i < N; ++i)
		for (j = 0; j < N; ++j)
			BT[i][j] = B[j][i];

	for (i = 0; i < N; ++i)
		for (j = 0; j < N; ++j) {
			C[i][j] = 0.0;
			for (k = 0; k < N; ++k)
				C[i][j] += A[i][k] * BT[j][k];
		}

	free(BT);
	
	end_time = clock();
	exec_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

	printf("Execution time for matmul(): %.6f seconds\n", exec_time);
}
