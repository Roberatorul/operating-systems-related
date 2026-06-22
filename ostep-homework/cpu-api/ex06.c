/*
6. Write a slight modification of the previous program, this time using waitpid() instead of wait(). When would waitpid() be
useful?
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
		exit(EXIT_SUCCESS);
	} else {
		printf("[Parent] Hello from parent!\n");
		int stat;
		int wc = waitpid(rc, &stat, 0);
		DIE(wc < 0, "wait");

		if (WIFEXITED(stat)) {
            printf("[Parent] Child process returned %d!\n", WEXITSTATUS(stat));
        } else {
            printf("[Parent] Child did not terminate normally.\n");
        }
	}

	return 0;
}
