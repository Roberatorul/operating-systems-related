#ifndef SERIAL_COUNTER_H
#define SERIAL_COUNTER_H

#include <unistd.h>

typedef struct __serial_counter_t {
	int value;
} serial_counter_t;

void sinit(serial_counter_t *c);
void sincr(serial_counter_t *c);
void sdecr(serial_counter_t *c);
int sget(serial_counter_t *c);

#endif // SERIAL_COUNTER_H
