/*- -*- mode: c; c-basic-offset: 8; -*-
 *
 * A Red-Black SOR.
 * Solves a N by N array.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "csapp.h"

/* Shared variables. */

int	iterations = 10; 	/* Number of iterations. */
int	N = 2000;			/* Default number of rows/columns. */
int num_threads = 4;	/* Default number of threads. */
float **array;			/* floating point grid. */
pthread_barrier_t barrier; /* pthread barrier. */

/* Function prototype(s). */
void *run_thread(void *arg);

/* Update value of a block to be the average value of its four neighbors. */
void
update_val(int i, int j)
{
	array[i][j] = (array[i - 1][j] + array[i + 1][j] + array[i][j - 1] +
			array[i][j + 1]) / (float) 4.0;
}

/*
 * The first row's index odd.
 */
void	
sor_first_row_odd(int first_row, int end)
{
	int	i, j, k;

	for (i = 0; i < iterations; i++) {

		/* Update red blocks. */
		for (j = first_row; j <= end; j++) {
		
			for (k = 1; k < N - 1; k += 2)
				update_val(j, k);

			if ((j += 1) > end)
				break;

			for (k = 2; k < N - 1; k += 2)
				update_val(j, k);
		}

		/* Update black blocks. */
		for (j = first_row; j <= end; j++) {

			for (k = 2; k < N - 1; k += 2)
				update_val(j, k);

			if ((j += 1) > end)
				break;
	
			for (k = 1; k < N - 1; k += 2)
				update_val(j, k);
		}
	}
}

/*
 * The first row's index is even.
 */
void	
sor_first_row_even(int first_row, int end)
{
	int	i, j, k;

	for (i = 0; i < iterations; i++) {

		/* Update red blocks. */
		for (j = first_row; j <= end; j++) {

			for (k = 2; k < N - 1; k += 2)
				update_val(j, k);

			if ((j += 1) > end)
				break;

			for (k = 1; k < N - 1; k += 2)
				update_val(j, k);
		}
	
		/* Update black blocks. */
		for (j = first_row; j <= end; j++) {
			
			for (k = 1; k < N - 1; k += 2)
				update_val(j, k);

			if ((j += 1) > end)
				break;

			for (k = 2; k < N - 1; k += 2)
				update_val(j, k);

		}				
	}
}

int
main(int argc, char **argv)
{
	int	c, i, j;
	pthread_t *threads;
	int *arg;
	float sum = 0;

	/* Process the command line options. */
	while ((c = getopt(argc, argv, "i:n:t:")) != -1)
		switch (c) {
		case 'i':
			iterations = atoi(optarg);
			break;
		case 'n':
			N = atoi(optarg);
			break;
		case 't':
			num_threads = atoi(optarg);
		}

	/* Initialize pthread variables. */
	Pthread_barrier_init(&barrier, NULL, num_threads);

	printf("Red-Black SOR: %d thread(s) running %d iterations over a %d by %d array\n",
		num_threads, iterations, N, N);

	/* Allocate the grid. */
	array = (float **)Malloc(N * sizeof(float *));

	for (i = 0; i < N; i++)
		array[i] = (float *)Malloc(N * sizeof(float));

	/* Allocate threads and start them. */
	threads = Malloc(num_threads * sizeof(pthread_t));

	for (i = 0; i < num_threads; i++) {

		arg = Malloc(sizeof(int));
		*arg = i;

		Pthread_create(&threads[i], NULL, run_thread, arg);
	}

	/* Wait for threads to finish. */
	for (i = 0; i < num_threads; i++)
		Pthread_join(threads[i], NULL);

	/* Find sum of all blocks in the grid. */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			sum += array[i][j];

	printf("Sum of all blocks in the grid after red-black SOR - %f\n", sum);

	return 0;
}

/* Each thread executes this function. */
void *
run_thread(void *arg)
{
	int i, j;

	/* Find region in grid which must be processed by this thread. */
	int first_row = (N * *((int *)arg)) / num_threads;
	int last_row = (N * (*((int *)arg) + 1)) / num_threads - 1;

	for (i = first_row; i <= last_row; i++) {

		/*
		 * Initialize top edge.
		 */
		if (i == 0) {
			for (j = 0; j < N; j++)
				array[0][j] = (float) 1.0;

			first_row = 1;
		}

		/*
		 * Initialize left and right edges.
		 */
		array[i][0] = (float) 1.0;
		array[i][N] = (float) 1.0;
		
		/*
		 * Initialize bottom edge.
		 */
		if (i == N - 1) {
			for (j = 0; j < N; j++)
				array[N - 1][j] = (float) 1.0;

			last_row = N - 2;
		}
	}

	/* Start relaxation process. */
	if (first_row & 1)
		sor_first_row_odd(first_row, last_row);
	else
		sor_first_row_even(first_row, last_row);

	Free(arg);
	return NULL;
}
