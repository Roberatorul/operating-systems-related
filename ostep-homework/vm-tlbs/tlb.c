#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define PAGE_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./tlb <num_pages_to_touch> <num_trials>\n");
        return EXIT_FAILURE;
    }

    int num_pages = atoi(argv[1]);
    int num_trials = atoi(argv[2]);

    if (num_pages <= 0 || num_trials <= 0) {
        fprintf(stderr, "Error: num_pages_to_touch and num_trials must be > 0.\n");
        return EXIT_FAILURE;
    }

    size_t total_bytes = (size_t)num_pages * PAGE_SIZE;
    int *buffer = (int *)malloc(total_bytes);
    if (buffer == NULL) {
        perror("Alocarea a esuat");
        return EXIT_FAILURE;
    }

    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int t = 0; t < num_trials; t++) {
        for (int i = 0; i < num_pages * (PAGE_SIZE / sizeof(int)); i += (PAGE_SIZE / sizeof(int))) {
            buffer[i] += 1; 
        }
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;

    if (microseconds < 0) {
        seconds -= 1;
        microseconds += 1000000;
    }

    long long total_time_ns = (seconds * 1000000000LL) + (microseconds * 1000LL);

    long long total_accesses = (long long)num_pages * num_trials;

    double time_per_access = (double)total_time_ns / total_accesses;

    printf("Average time per access: %f ns\n", time_per_access);

    free(buffer);

    return 0;
}