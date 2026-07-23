#ifndef SLOPPY_COUNTER_H
#define SLOPPY_COUNTER_H

#include <pthread.h>

#define NUMCPUS 8

typedef struct __sloppy_counter_t {
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // per-CPU locks
    int threshold;                  // update frequency
} sloppy_counter_t;

void sloppy_init(sloppy_counter_t *c, int threshold);
void sloppy_update(sloppy_counter_t *c, int threadID, int amt);
int sloppy_get(sloppy_counter_t *c);

#endif // SLOPPY_COUNTER_H