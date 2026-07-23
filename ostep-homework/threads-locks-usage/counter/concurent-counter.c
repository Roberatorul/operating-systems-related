#include "concurent-counter.h"
#include "../utils/utils.h"

void init(counter_t* c) {
	c->value = 0;
	int rc = pthread_mutex_init(&c->lock, NULL);

	DIE(rc != 0, "pthread_mutex_init");
}

void increment(counter_t* c) {
	pthread_mutex_lock(&c->lock);
	c->value++;
	pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t* c) {
	pthread_mutex_lock(&c->lock);
	c->value--;
	pthread_mutex_unlock(&c->lock);
}

int get(counter_t* c) {
	pthread_mutex_lock(&c->lock);
	int rc = c->value;
	pthread_mutex_unlock(&c->lock);

	return rc;
}
