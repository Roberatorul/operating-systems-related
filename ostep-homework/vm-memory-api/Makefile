CC = gcc
CFLAGS = -Wall -Wextra -g

all: null memory-leak invalid-access use-after-free

null: null.c
	$(CC) $(CFLAGS) null.c -o null

memory-leak: memory-leak.c
	$(CC) $(CFLAGS) memory-leak.c -o memory-leak

invalid-access: invalid-access.c
	$(CC) $(CFLAGS) invalid-access.c -o invalid-access

use-after-free: use-after-free.c
	$(CC) $(CFLAGS) use-after-free.c -o use-after-free

clean:
	rm -rf null memory-leak invalid-access use-after-free

.PHONY: all clean
