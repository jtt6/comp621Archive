#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>

// Large enough for timing to be accurate and to overwhelm cache capacity.
#define SIZE 10000000
#define PARTIAL_SIZE 300000
int *data;

// access all data pages sequentially
void access_sequential(int *data, int size) {
	int i;
	int running = 0;

	for (i = 0; i < size; i++) {
		switch (rand() % 3) {
		case 0:
			// read
			running += data[i];
			break;
		case 1:
			// write
			data[i] = running;
			break;
		case 2:
			// no access
			break;
		}
	}
}

// access all data pages in random order
void access_random(int *data, int size) {
	int i, location;
	int running = 0;

	for (i = 0; i < size; i++) {
		location = rand() % size;
		switch (rand() % 3) {
		case 0:
			// read
			running += data[location];
			break;
		case 1:
			// write
			data[location] = running;
			break;
		case 2:
			// no access
			break;
		}
	}
}

// access some data pages randomly
void access_partial(int *data, int size, int max_i) {
	int i, location;
	int running = 0;

	// Only access data[0]..data[max_location-1]

	for (i = 0; i < size; i++) {
		location = rand() % max_i;
		switch (rand() % 3) {
		case 0:
			// read
			running += data[location];
			break;
		case 1:
			// write
			data[location] = running;
			break;
		case 2:
			// no access
			break;
		}
	}
}

int main() {
	int i;
	clock_t start, end;


        // Initialize data
	printf("Allocating.\n");
	data = (int *) mmap(NULL, SIZE * sizeof(int),
			    PROT_READ | PROT_WRITE,
			    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	printf("Initializing.\n");
	for (i = 0; i < SIZE; i++) {
		data[i] = rand();
	}

	// Speed of each of the following is not dependent on data values,
	// so we don't bother re-initializing the data.
	printf("Testing.\n");

	start = clock();
        madvise(data, SIZE * sizeof(int), MADV_SEQUENTIAL);
	access_sequential(data, SIZE);
	end = clock();
	printf("Sequential: %f\n", ((double)(end - start)) / CLOCKS_PER_SEC);

	start = clock();
	madvise(data, SIZE * sizeof(int), MADV_RANDOM);
	access_random(data, SIZE);
	end = clock();
	printf("Random: %f\n", ((double)(end - start)) / CLOCKS_PER_SEC);

	start = clock();
	madvise(data, PARTIAL_SIZE * sizeof(int), MADV_RANDOM);
	madvise(data + PARTIAL_SIZE * sizeof(int), (SIZE - PARTIAL_SIZE ) * sizeof(int), MADV_DONTNEED);
	access_partial(data, SIZE, PARTIAL_SIZE);
	end = clock();
	printf("Partial: %f\n", ((double)(end - start)) / CLOCKS_PER_SEC);
}
