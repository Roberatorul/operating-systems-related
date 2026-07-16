#include <stdio.h>
#include <stdlib.h>

#define NUM_ELEMENTS 100

int main() {
	int *data = malloc(NUM_ELEMENTS * sizeof(int));

	for (size_t i = 0; i < 99; i++)
		data[i] = (int)i;

	/* ex6 */
	// free(data);

	// printf("This should result in a seg fault: %d.\n ", data[10]);
	// printf("But the OS doesn't actually free the page imidiatly after calling free\n");

	/* ex7 */
	free(&data[5]);
	return 0;
}
