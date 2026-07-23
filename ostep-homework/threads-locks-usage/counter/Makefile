CC = gcc
CFLAGS = -Wall -Wextra

all: test-counter

test-counter: test-counter.c serial-counter.c concurent-counter.c sloppy-counter.c tls-counter.c
	$(CC) $(CFLAGS) $^ -o $@ -pthread

clean:
	rm -f test-counter results.csv

.PHONY: all clean