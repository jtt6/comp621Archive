#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>

volatile long count = 0; /* Shared */

void *thread(void* vargp){
	long i;
	long niters = *((long*) vargp);

	for (i = 0; i < niters; i++)
		count++;

	return NULL;
}

int main(int argc, char **argv){
	long niters = atoi(argv[1]);
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL,
			thread, &niters);
	pthread_create(&tid2, NULL,
			thread, &niters);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	printf("count=%ld\n", count);
	exit(0);
}
