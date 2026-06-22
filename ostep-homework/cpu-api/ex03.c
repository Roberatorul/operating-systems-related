/*
3. Write another program using fork(). The child process should
print “hello”; the parent process should print “goodbye”. You should
try to ensure that the child process always prints first; can you do
this without calling wait() in the parent?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

int main() {
	/* Method 1 */
	printf("======= Method1 =======\n");
	int fd[2];
	pipe(fd); /* fd[0] = read, fd[1] = write */

	int rc = fork();
	DIE(rc < 0, "fork");

	if (rc == 0) {
        printf("hello\n");
        write(fd[1], "x", 1); /* give signal to parent */
		exit(0);
    } else if (rc > 0) {
        char buffer;
        read(fd[0], &buffer, 1); /* blocks until a char arrives */
        printf("goodbye\n");
		wait(NULL);
    }
	printf("\n");

	/* Method 2 (not recommended) */
	printf("======= Method2 =======\n");
	rc = vfork();
	DIE(rc < 0, "vfork");
	if (rc == 0) {
		printf("hello\n");
		_exit(0);
	} else {
		printf("goodbye\n");
	}
	return 0;
}
