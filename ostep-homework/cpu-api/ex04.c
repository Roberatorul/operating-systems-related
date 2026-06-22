/*
4. Write a program that calls fork() and then calls some form of
exec() to run the program /bin/ls. See if you can try all of the
variants of exec(), including (on Linux) execl(), execle(),
execlp(), execv(), execvp(), and execvpe(). Why do
you think there are so many variants of the same basic call?
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

int main() {
	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "fork failed!\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		char* my_args[2];
		my_args[0] = strdup("ls");
		my_args[1] = NULL;
		execvp(my_args[0], my_args);

		perror("execv failed");
	} else {
		wait(NULL);
	}

	return 0;
}
