/*
1. Write a program that calls fork(). Before calling fork(), have the
main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process?
What happens to the variable when both the child and parent change
the value of x?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	int x = 100;

	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "fork failed!\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		printf("[Child] %d\n", x);
		x++;
		printf("[Child] %d\n", x);
	} else {
		wait(NULL);
		printf("[Parent] %d\n", x);
	}

	return 0;
}
