// SPDX-License-Identifier: BSD-3-Clause

#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <internal/arch/x86_64/syscall_list.h>

int open(const char *filename, int flags, ...)
{
    mode_t mode = 0;

    if (flags & O_CREAT) {
        va_list valist;
        va_start(valist, flags);
        mode = va_arg(valist, mode_t);
        va_end(valist);
    }

    long rc = syscall(__NR_open, (long)filename, (long)flags, (long)mode);

    if (rc < 0) {
        errno = -rc;
        return -1;
    }

    return (int)rc;
}
