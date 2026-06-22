/*
2. Write a program that opens a file (with the open() system call)
and then calls fork() to create a new process. Can both the child
and parent access the file descriptor returned by open()? What
happens when they are writing to the file concurrently, i.e., at the
same time?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#include "common.h"

#define BUF_SIZE 1024

void write_to_file(int dest_file_fd, int src_file_fd) {
	int bytes_read;
	int rc;
	char buffer[BUF_SIZE];

	while (1) {
		bytes_read = read(src_file_fd, buffer, sizeof(buffer) - 1);
		DIE(bytes_read < 0, "read failed");

		if (bytes_read == 0) /* EOF reached */
			break;

		rc = write(dest_file_fd, buffer, bytes_read);
		DIE(rc < 0, "write failed");
	}
}

int main() {
	int output_file = open("a.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);

	int rc = fork();
	if (rc < 0) {
		fprintf(stderr, "fork failed!\n");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		printf("[Child] output_file = %d\n", output_file);

		int fd = open("lorem1.txt", O_RDONLY);
		DIE(fd < 0, "open failed");

		write_to_file(output_file, fd);
	} else {
		printf("[Parent] output_file = %d\n", output_file);

		int fd = open("lorem2.txt", O_RDONLY);
		DIE(fd < 0, "open failed");

		write_to_file(output_file, fd);

		wait(NULL);
	}

	return 0;
}
