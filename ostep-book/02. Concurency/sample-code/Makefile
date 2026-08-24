CC = gcc
CFLAGS = -Wall -Wextra
EXECUTABLES = semaphore-ordering semaphore-lock threads-locks

all: $(EXECUTABLES)

semaphore-ordering: semaphore-ordering.c
	$(CC) $(CFLAGS) $< -o $@ -pthread

semaphore-lock: semaphore-lock.c
	$(CC) $(CFLAGS) $< -o $@ -pthread

threads-locks: threads-locks.c
	$(CC) $(CFLAGS) $< -o $@ -pthread

clean:
	rm -rf $(EXECUTABLES)

.PHONY: all clean