// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/arch/x86_64/syscall_list.h>

int fstat(int fd, struct stat *st)
{
    long rc = syscall(__NR_fstat, fd, st);

    if (rc < 0) {
        errno = -rc;
        return -1;
    }

    return 0;
}
