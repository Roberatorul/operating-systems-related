// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>
#include <internal/arch/x86_64/syscall_list.h>

int close(int fd)
{
	return syscall(__NR_close, fd);
}
