#include "tls-counter.h"

__thread int private_local_count = 0;

void tls_init(tls_counter_t *c, int threshold) {
    c->global = 0;
    c->threshold = threshold;
    pthread_mutex_init(&c->glock, NULL);
}

void tls_update(tls_counter_t *c, int amt) {
    private_local_count += amt;

    if (private_local_count >= c->threshold) {
        pthread_mutex_lock(&c->glock);
        c->global += private_local_count;
        pthread_mutex_unlock(&c->glock);

        private_local_count = 0;
    }
}

void tls_flush(tls_counter_t *c) {
    if (private_local_count > 0) {
        pthread_mutex_lock(&c->glock);
        c->global += private_local_count;
        pthread_mutex_unlock(&c->glock);
        private_local_count = 0;
    }
}

int tls_get(tls_counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val;
}