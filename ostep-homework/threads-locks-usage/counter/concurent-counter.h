#ifndef CONCURENT_COUNTER_H
#define CONCURENT_COUNTER_H

#include <stdlib.h>
#include <pthread.h>

typedef struct __counter_t {
	int value;
	pthread_mutex_t lock; // 0 - unlocked; 1 - locked
} counter_t;

void init(counter_t* c);
void increment(counter_t* c);
void decrement(counter_t* c);
int get(counter_t* c);

#endif // CONCURENT_COUNTER_H
