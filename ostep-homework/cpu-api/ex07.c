/*
7. Write a program that creates a child process, and then in the child
closes standard output (STDOUT_FILENO). What happens if the child
calls printf() to print some output after closing the descriptor?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

int main() {
	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "fork failed!\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		printf("[Parent] Hello from child!\n");
		close(STDOUT_FILENO);
		printf("[Parent] Hello from child!\n"); /* Will return EBADF */
		exit(EXIT_SUCCESS);
	} else {
		printf("[Parent] Hello from parent!\n");
		int stat;
		int wc = waitpid(rc, &stat, 0);
		DIE(wc < 0, "wait");

		printf("[Parent] Hello from parent!\n"); 

		if (WIFEXITED(stat)) {
            printf("[Parent] Child process returned %d!\n", WEXITSTATUS(stat));
        } else {
            printf("[Parent] Child did not terminate normally.\n");
        }
	}

	return 0;
}
