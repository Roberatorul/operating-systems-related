#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 8
#define NUM_UPDATES_PER_THREAD 12345678

unsigned long long value = 0;
sem_t s;

/* Increment the global variable value by one */
void incr() {
	sem_wait(&s);
	value++;
	sem_post(&s);
}

void *worker(void *args) {
	unsigned long long num_rounds = (unsigned long long)args;

	for (unsigned long long i = 0; i < num_rounds; i++)
		incr();

	return NULL;
}

int main() {
	int X = 1;
	pthread_t tids[NUM_THREADS];

	sem_init(&s, 0, X);

	printf("Start executing code...\n");
	for (int i = 0; i < NUM_THREADS; i++)
		pthread_create(&tids[i], NULL, worker, (void *)NUM_UPDATES_PER_THREAD);

	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(tids[i], NULL);

	printf("Expected: %lld\n", (unsigned long long)NUM_UPDATES_PER_THREAD * NUM_THREADS);
	printf("Returned: %lld\n", value);

	return EXIT_SUCCESS;
}
