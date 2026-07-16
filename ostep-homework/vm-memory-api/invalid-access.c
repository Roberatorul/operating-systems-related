#include <stdio.h>
#include <stdlib.h>

#define NUM_BYTES 100

int main() {
	char *data = malloc(NUM_BYTES);
	data[100] = 'a';
	printf("data[100] = %c\n", data[100]);

	/* Comment the line abve and use this to
	 solve the memory leak in valgirnd */
	// data[99] = 'a';
	// printf("data[99] = %c\n", data[99]);

	free(data);
	return 0;
}
