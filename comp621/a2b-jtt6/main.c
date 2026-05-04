#include "csapp.h"

/*
   Matrix multiplication, defined in external file.
   N = size -- assumes all matrices square
   A,B = inputs
   C = output, not assumed initialized
*/
void matmul(int N, float (*A)[N], float (*B)[N], float (*C)[N]);

/* Read from file.
   N = size -- assumes all matrices square
   A, B = inputs, not assumed initialized
   C = output, not assumed initialized
*/
void readmatrix(int N, float (*A)[N], float (*B)[N], const char *filename) {
	int i, j, cnt;
	FILE *fp;

	fp = Fopen(filename, "r");
	// Read A.
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			cnt = fscanf(fp, "%f", A[i] + j);
			if (cnt <= 0)
				app_error("fscanf error");
		}
	}
	// Read B.
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			cnt = fscanf(fp, "%f", B[i] + j);
			if (cnt <= 0)
				app_error("fscanf error");
		}
	}
	Fclose(fp);
}

/* Print matrix to standard out.
   N = size -- assumes square matrix
   A = input matrix
*/
void print_matrix(int N, float (*A)[N], FILE *fp) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j)
			fprintf(fp, "%3lf ", A[i][j]);
		fprintf(fp, "\n");
	}
}

void usage_error(char *argv0) {
	fprintf(stderr,
		"Usage: %s <1000 | 2000> <outputfile>\n"
		"\tThe number denotes the matrix size.\n",
		argv0);
	exit(1);
}

int main(int argc, char *argv[]) {
	int N;
	char filename[16];
	FILE *outfp;

	if (argc != 3)
		usage_error(argv[0]);

	// Only accept 1000 or 2000 as arguments.
	if (!strcmp(argv[1], "1000") || !strcmp(argv[1], "2000")) {
		N = atoi(argv[1]);
		snprintf(filename, 15, "matrix%s.dat", argv[1]);
	} else {
		usage_error(argv[0]);
	}

	/* Variable length array. Need to define here instead of
	start of function because N must be initialized before use. */
	float(*A)[N], (*B)[N], (*C)[N];

	A = Malloc(sizeof(float[N][N]));
	B = Malloc(sizeof(float[N][N]));
	C = Malloc(sizeof(float[N][N]));

	readmatrix(N, A, B, filename);

	matmul(N, A, B, C);

	outfp = Fopen(argv[2], "w");
	print_matrix(N, C, outfp);
	Fclose(outfp);

	Free(A);
	Free(B);
	Free(C);

	return 0;
}
