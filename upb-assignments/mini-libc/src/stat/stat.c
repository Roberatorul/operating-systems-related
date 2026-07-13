// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <internal/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <internal/arch/x86_64/syscall_list.h>

int stat(const char *restrict path, struct stat *restrict buf)
{
    long rc = syscall(__NR_stat, path, buf);

    if (rc < 0) {
        errno = -rc;
        return -1;
    }

    return 0;
}

