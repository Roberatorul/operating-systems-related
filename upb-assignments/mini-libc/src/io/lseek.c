// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <errno.h>
#include <internal/arch/x86_64/syscall_list.h>
#include <internal/types.h>

off_t lseek(int fd, off_t offset, int whence)
{
    long rc = syscall(__NR_lseek, fd, offset, whence);

    if (rc < 0) {
        errno = -rc;
        return -1;
    }

    return rc;
}
