#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s;

void *child(void *args) {
	(void) args;
	sleep(3); // To se if the child executes first
	printf("child\n");
	sem_post(&s);

	return NULL;
} 

int main() {
	int X = 0;
	pthread_t tid;

	sem_init(&s, 0, X);

	printf("Start executing code...\n");
	pthread_create(&tid, NULL, child, NULL);
	sem_wait(&s);
	printf("parent\n");

	return EXIT_SUCCESS;
}
