#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMCPUS 4
#define NUM_THREADS 8
#define NUM_UPDATES_PER_THREAD 1234567

typedef struct __counter_t {
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int threshold;                  // update freq
} counter_t;

void init(counter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    int i;
    for (i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

void update(counter_t *c, int threadID, int amt) {
    int cpu = threadID % NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold) {
        // transfer to global (assumes amt>0)
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}

counter_t my_counter;

void* worker(void* arg) {
    long threadID = (long)arg; 

    for (int i = 0; i < NUM_UPDATES_PER_THREAD; i++) {
        update(&my_counter, threadID, 1);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./approximate-counter <threshold>\n");
		return EXIT_FAILURE;
	}

    int threshold = atoi(argv[1]); 
    pthread_t threads[NUM_THREADS];

    init(&my_counter, threshold);
    printf("Initialized counter with threshold = %d\n", threshold);
	
    for (long i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
    }

    int expected_value = NUM_THREADS * NUM_UPDATES_PER_THREAD;
    int global_value = get(&my_counter);

	FILE* output_file = fopen("results.csv", "a");
	if (output_file != NULL) {
        fprintf(output_file, "%d,%d,%d\n", threshold, expected_value, global_value);
        fclose(output_file);
    }

	printf("Program finished. View results in results.csv file\n");

    return EXIT_SUCCESS;
}