#include "stdio.h"
#include "string.h"
#include "internal/io.h"

#define STDIN_FILENO 1

int puts(const char *str)
{
    size_t len = strlen(str);

    if (write(STDIN_FILENO, str, len) < 0)
        return -1;

    if (write(STDIN_FILENO, "\n", 1) < 0)
        return -1;

    return 0;
}
