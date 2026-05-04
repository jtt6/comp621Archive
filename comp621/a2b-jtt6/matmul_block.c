#include <unistd.h>
#include <time.h>
#include <stdio.h>
int min(int a, int b) { return (a < b) ? a : b; }

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
	int jj, kk;
	float sum;
	clock_t start_time, end_time;
	double exec_time;
	
	start_time = clock();

	long linesize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE) / sizeof(float);

	for (jj = 0; jj < N; jj += linesize) {
		for (kk = 0; kk < N; kk += linesize) {

			for (i = 0; i < N; i++) {
				for (j = jj; j < min(jj + linesize, N); j++) {
					sum = 0.0;
					for (k = kk; k < min(kk + linesize, N);
					     k++) {
						sum += A[i][k] * B[k][j];
					}
					C[i][j] += sum;
				}
			}
		}
	}

	end_time = clock();
	exec_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
	
	printf("Execution time for matmul(): %.6f seconds\n", exec_time);
}
