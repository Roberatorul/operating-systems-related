#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "serial-counter.h"
#include "concurent-counter.h"
#include "sloppy-counter.h"
#include "tls-counter.h"

#define NUM_ROUNDS (100000000) 

#define SLOPPY_THRESHOLD 1024 

typedef struct {
    counter_t *c;
    int loops;
} thread_arg_concurent_t;

typedef struct {
    sloppy_counter_t *sc;
    int loops;
    int thread_id;
} thread_arg_sloppy_t;

typedef struct {
    tls_counter_t *tc;
    int loops;
} thread_arg_tls_t;

void *worker_concurent(void *args) {
    thread_arg_concurent_t *targs = (thread_arg_concurent_t *)args;

    for (int i = 0; i < targs->loops; i++) {
        increment(targs->c);
    }

    return NULL;
}

void *worker_sloppy(void *args) {
    thread_arg_sloppy_t *targs = (thread_arg_sloppy_t *)args;
    
    for (int i = 0; i < targs->loops; i++) {
        sloppy_update(targs->sc, targs->thread_id, 1);
    }
    
    return NULL;
}

void *worker_tls(void *args) {
    thread_arg_tls_t *targs = (thread_arg_tls_t *)args;
    
    for (int i = 0; i < targs->loops; i++) {
        tls_update(targs->tc, 1);
    }

    tls_flush(targs->tc); 

    return NULL;
}

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./test-counter <num_threads>\n");
        return EXIT_FAILURE;
    }

	/* Sanity check */
    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "num_threads must be > 0\n");
        return EXIT_FAILURE;
    }

	/* Time measurement variables */
    struct timespec start, end;
    int expected = NUM_ROUNDS;

    int loops_per_thread = NUM_ROUNDS / num_threads;
    int remainder = NUM_ROUNDS % num_threads;

	/* Initialize counters */
    serial_counter_t serial_counter;
    counter_t concurent_counter;
    sloppy_counter_t sloppy_counter;
	tls_counter_t tls_counter;

    sinit(&serial_counter);
    init(&concurent_counter);
    sloppy_init(&sloppy_counter, SLOPPY_THRESHOLD);
    tls_init(&tls_counter, SLOPPY_THRESHOLD);

	/* ====== Start measuring serial implementation ====== */
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_ROUNDS; i++) {
        sincr(&serial_counter);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_serial = get_time_diff(start, end);
    int val_serial = sget(&serial_counter);
	/* ====== End measuring serial implementation ====== */

	/* ====== Start measuring concurent implementation ====== */
    pthread_t tid_concurent[num_threads];
    thread_arg_concurent_t targs_concurent[num_threads];

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++) {
        targs_concurent[i].c = &concurent_counter;
        targs_concurent[i].loops = loops_per_thread + (i == 0 ? remainder : 0); 

        pthread_create(&tid_concurent[i], NULL, worker_concurent, &targs_concurent[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid_concurent[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_concurent = get_time_diff(start, end);
    int val_concurent = get(&concurent_counter);
	/* ====== End measuring concurent implementation ====== */

	/* ====== Start measuring sloppy implementation ====== */
    pthread_t tid_sloppy[num_threads];
    thread_arg_sloppy_t targs_sloppy[num_threads];

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++) {
        targs_sloppy[i].sc = &sloppy_counter;
        targs_sloppy[i].loops = loops_per_thread + (i == 0 ? remainder : 0);
        targs_sloppy[i].thread_id = i;
        
        pthread_create(&tid_sloppy[i], NULL, worker_sloppy, &targs_sloppy[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid_sloppy[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_sloppy = get_time_diff(start, end);
    int val_sloppy = sloppy_get(&sloppy_counter);
	/* ====== End measuring sloppy implementation ====== */

	/* ====== Start measuring tls implementation ====== */
	pthread_t tid_tls[num_threads];
    thread_arg_tls_t targs_tls[num_threads];

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < num_threads; i++) {
        targs_tls[i].tc = &tls_counter;
        targs_tls[i].loops = loops_per_thread + (i == 0 ? remainder : 0);
        
        pthread_create(&tid_tls[i], NULL, worker_tls, &targs_tls[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid_tls[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_tls = get_time_diff(start, end);
    int val_tls = tls_get(&tls_counter);
	/* ====== End measuring tls implementation ====== */

    printf("[Done] Threads: %-2d | Serial: %.4fs | Concurent: %.4fs | Sloppy: %.4fs | TLS: %.4fs\n", 
           num_threads, time_serial, time_concurent, time_sloppy, time_tls);

    FILE* output_file = fopen("results.csv", "a");
    if (output_file != NULL) {
        fprintf(output_file, "%d,%d,%d,%.6f,%d,%.6f,%d,%.6f,%d,%.6f\n", 
                num_threads, 
                expected, 
                val_serial, 
                time_serial, 
                val_concurent, 
                time_concurent,
                val_sloppy,
                time_sloppy,
                val_tls,
                time_tls);
        fclose(output_file);
    } else {
        fprintf(stderr, "Can't open results.csv\n");
		return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}