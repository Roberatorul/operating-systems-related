#include <stdio.h>
#include <stdlib.h>

#define NUM_BYTES 1024

int main() {
	char *ptr = malloc(NUM_BYTES);
	ptr[0] = 'a';

	return 0;
}
