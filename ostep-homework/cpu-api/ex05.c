/*
5. Now write a program that uses wait() to wait for the child process
to finish in the parent. What does wait() return? What happens if
you use wait() in the child?
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
		printf("[Child] Hello from child!\n");
		int wc = wait(NULL);
		if (wc < 0)
			printf("[Child] No process to wait for!\n");
		exit(EXIT_SUCCESS);
	} else {
		printf("[Parent] Hello from parent!\n");
		int stat;
		int wc = wait(&stat);
		DIE(wc < 0, "wait");

        if (WIFEXITED(stat)) {
            printf("[Parent] Child process returned %d!\n", WEXITSTATUS(stat));
        } else {
            printf("[Parent] Child did not terminate normally.\n");
        }
	}

	return 0;
}
