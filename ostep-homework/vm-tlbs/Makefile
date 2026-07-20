CC = gcc
CFALGS = -O0 -Wall -Wextra

all: tlb

tlb: tlb.c
	$(CC) $(CFALGS) $< -o $@

clean:
	rm -rf tlb

.PHONY: all, clean