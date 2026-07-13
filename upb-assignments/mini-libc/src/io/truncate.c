// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/types.h>
#include <internal/arch/x86_64/syscall_list.h>

int truncate(const char *path, off_t length)
{
    long rc = syscall(__NR_truncate, path, length);

    if (rc < 0) {
        errno = -rc;
        return -1;
    }

    return 0;
}
