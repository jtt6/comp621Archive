/*- -*- mode: c; c-basic-offset: 8; -*-
 *
 * Echo key - using producer-consumer paradigm.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "csapp.h"

#define NITEMS 10			/* Number of items in shared buffer. */
#define NCONS  2                        /* Number of consumer threads. */

/* Shared variables. */
char shared_buffer[NITEMS];	/* Echo buffer. */
int shared_cnt;			/* Item count. */
unsigned int prod_index = 0;    /* Producer index into shared buffer. */ 
unsigned int cons_index = 0;    /* Consumer index into shared buffer. */

pthread_mutex_t mutex;		/* pthread mutex. */ 

/* DECLARE THE CONDITION VARIABLES HERE. */
/* FILL ME IN */
pthread_cond_t space, item;



void	*producer(void *arg);
void	*consumer(void *arg);

int
main(void)
{
	pthread_t prod_tid, cons_tid[NCONS];
	int * ip;

	printf("Type in characters.  They will be echoed by %d threads.\n", NCONS);


	/* Initialize pthread variables. */
	Pthread_mutex_init(&mutex, NULL);
	
	/* INITIALIZE THE CONDITION VARIABLES HERE. */
	/* FILL ME IN. */
	Pthread_cond_init(&space, NULL);
	Pthread_cond_init(&item, NULL);
	
	/* Start producer thread. */
	Pthread_create(&prod_tid, NULL, producer, NULL);

	/* Start consumer threads. */
	for (int i=0; i<NCONS; i++) {
		ip = (int *) Malloc(sizeof(int));
		*ip = i;
		Pthread_create(&cons_tid[i], NULL, consumer, (void *) ip);
	}
	
	/* Wait for threads to finish. */ 
	Pthread_join(prod_tid, NULL);
	for (int i=0; i<NCONS; i++) {
		Pthread_join(cons_tid[i], NULL);
	}
			
	/* Clean up. */
	Pthread_mutex_destroy(&mutex);
	
	/* DESTROY THE CONDITION VARIABLES HERE. */
	/* FILL ME IN. */
	Pthread_cond_destroy(&space);
	Pthread_cond_destroy(&item);

	return (0);
}

/* Producer thread executes this function. */
void *
producer(void *argp)
{
	char key;

        /* Dummy assignment to eliminate warnings.
	 * argp is unused because no argument is being passed into function. */
        argp = (void *)argp;

	while (true) {
		/* Read input key.  Ignore return value. */
		/* Using scanf, rather than getchar, to ignore special characters. */
		(void) scanf("%c", &key);

		/* MODIFY THE CODE BELOW TO LOCK AND WAIT ON APPROPRIATE CONDITION VARIABLE */
	//	while (true) {
			/* Acquire mutex lock. */
			Pthread_mutex_lock(&mutex);

			/* If buffer is full, release mutex lock and check again. */
			while (shared_cnt == NITEMS)
				// Pthread_mutex_unlock(&mutex);
				Pthread_cond_wait(&space, &mutex);
		//}

		/* Store key in shared buffer. */ 
		shared_buffer[prod_index] = key;

		/* SIGNAL THE APPROPRIATE CONDITION VARIABLE HERE. */
		/* FILL ME IN. */
		Pthread_cond_signal(&item);

		/* Update shared count variable. */
		shared_cnt++;

		/* Update producer index. */
		if (prod_index == NITEMS - 1)
			prod_index = 0;
		else
			prod_index++;
		
		/* Release mutex lock. */
		Pthread_mutex_unlock(&mutex); 
	}
	return (NULL);
}

/* Consumer thread executes this function. */
void *
consumer(void *argp)
{
	char key;
        int id = *(int *)argp;   /* The argument is the thread id. */
	Free(argp);

	while (true) {
		/* MODIFY THE LOOP BELOW TO LOCK AND WAIT ON APPROPRIATE CONDITION VARIABLE */
	//	while (true) {
			/* Acquire mutex lock. */
			Pthread_mutex_lock(&mutex);

			/* If buffer is empty, release mutex lock and check again. */
			while (shared_cnt == 0)
				// Pthread_mutex_unlock(&mutex);
				Pthread_cond_wait(&item, &mutex);
	//	}

		/* Read key from shared buffer. */ 
		key = shared_buffer[cons_index];
		
		/* Echo key. */
		printf("consumer tid=%d key=%c\n", id, key);

		/* SIGNAL THE APPROPRIATE CONDITION VARIABLE HERE. */
		/* FILL ME IN. */
		Pthread_cond_signal(&space);

		/* Update shared count variable. */
		shared_cnt--;

		/* Update consumer index. */
		if (cons_index == NITEMS - 1)
			cons_index = 0;
		else
			cons_index++;
	
		/* Release mutex lock. */
		Pthread_mutex_unlock(&mutex);
	}
	return (NULL);
}
