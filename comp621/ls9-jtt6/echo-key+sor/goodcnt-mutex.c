#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NITERS 10000000		/* Number of iterations. */
unsigned int cnt = 0; 		/* Shared count variable. */
pthread_mutex_t mutex;		/* Shared pthread mutex. */ 

/* Function prototypes. */
void *count(void *arg);

int 
main() 
{ 
	pthread_t tid1, tid2; 

	/* Initialize pthread variables. */
	Pthread_mutex_init(&mutex, NULL);

	/* Start threads. */
	Pthread_create(&tid1, NULL, count, NULL);
	Pthread_create(&tid2, NULL, count, NULL);

	/* Wait for threads to finish. */
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
			
	if (cnt != (unsigned)NITERS * 2)
		printf("BOOM! cnt = %d\n", cnt); 
	else
		printf("OK cnt = %d\n", cnt);

	/* Clean up. */
	Pthread_mutex_destroy(&mutex);

	return 0;
}

/* Each thread executes this function. */
void * 
count(void *arg)
{
	int i;

	for (i = 0; i < NITERS; i++) {
	
		/* Acquire mutex lock. */
		Pthread_mutex_lock(&mutex);
		cnt++;
		/* Release mutex lock. */
		Pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
