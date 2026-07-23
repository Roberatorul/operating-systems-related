#include "serial-counter.h"

void sinit(serial_counter_t *c) {
	c->value = 0;
}

void sincr(serial_counter_t *c) {
	c->value++;
}

void sdecr(serial_counter_t *c) {
	c->value--;
}

int sget(serial_counter_t *c) {
	return c->value;
}
