CC = gcc
CFLAGS = -Wall -Wextra

all: approximate-counter

approximate-counter: approximate-counter.c
	$(CC) $(CFLAGS) $< -o $@ -pthread

clean:
	rm -rf approximate-counter results.csv

.PHONY: all clean