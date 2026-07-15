#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MB_TO_BYTES (1024 * 1024)

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <num_of_mega_bytes> <num_itterations>", argv[0]);
		return EXIT_FAILURE;
	}

	int proc_id = getpid();
	printf("Process pid: %d\n\n", proc_id);

	size_t num_bytes = atoll(argv[1]) * MB_TO_BYTES; 
	size_t num_itterations = atoi(argv[2]);

	char* vec = malloc(num_bytes);
	if (vec == NULL) {
		fprintf(stderr, "malloc failed\n");
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < num_itterations; i++) {
		printf("Itteration %zu, touching the entire array\n", i);
		for (size_t j = 0; j < num_bytes; j++)
			vec[j] = 'a';
		printf("Itteration %zu finished\n", i);
	}
	return EXIT_SUCCESS;
}
