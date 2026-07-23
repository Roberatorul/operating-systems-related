#ifndef TLS_COUNTER_H
#define TLS_COUNTER_H

#include <pthread.h>

typedef struct __tls_counter_t {
    int global;
    pthread_mutex_t glock;
    int threshold;
} tls_counter_t;

void tls_init(tls_counter_t *c, int threshold);
void tls_update(tls_counter_t *c, int amt);
void tls_flush(tls_counter_t *c);
int tls_get(tls_counter_t *c);

#endif // TLS_COUNTER_H
