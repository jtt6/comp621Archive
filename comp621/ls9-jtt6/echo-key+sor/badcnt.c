#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NITERS 10000000		/* Number of iterations. */
unsigned int cnt = 0; 		/* Shared variable. */

/* Function prototypes. */
void *count(void *arg);

int 
main() 
{ 
	pthread_t tid1, tid2; 

	/* Start threads. */
	Pthread_create(&tid1, NULL, count, NULL);
	pthread_create(&tid2, NULL, count, NULL);

	/* Wait for threads to finish. */
	Pthread_join(tid1, NULL);
	Pthread_join(tid2, NULL);
			
	if (cnt != (unsigned)NITERS * 2)
		printf("BOOM! cnt = %d\n", cnt); 
	else
		printf("OK cnt = %d\n", cnt);
}

/* Each thread executes this function. */
void *
count(void *arg)
{
	int i;

	for (i = 0; i < NITERS; i++) 
		cnt++;

	return NULL;
}
