#include <stdio.h>
#include <stdlib.h>

int main() {
	int *ptr = NULL;
	printf ("This should crash: %d", *ptr);
	return 0;
}
