/*
8. Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the
pipe() system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

#define PROC_NUM 2

int main() {
	/* IMPORTANT!!! This needs to be before the processes are created */
	int pipefd[2];
	int rc = pipe(pipefd); /* pipefd[0] = read; pipefd[1] = write */
	DIE(rc < 0, "pipe");

	int rc1 = fork();
    if (rc1 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (rc1 == 0) {
        close(pipefd[0]); 

        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[1]); 

		printf("Hello! This message was sent through a pipe from Child 1\n");        
        exit(EXIT_SUCCESS);
    }

    int rc2 = fork();
    if (rc2 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (rc2 == 0) {
        close(pipefd[1]);

        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        char buffer[256];
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            // Folosim stderr pentru printare, pentru că stdout e pentru alte ieșiri normale
			/* We use stderr because stdout is for normal output */
			fprintf(stderr, "Child 2 got a message from STDIN: %s", buffer);
        }

        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(rc1, NULL, 0);
    waitpid(rc2, NULL, 0);

    return 0;
}
