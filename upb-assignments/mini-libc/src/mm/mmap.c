// SPDX-License-Identifier: BSD-3-Clause

#include <sys/mman.h>
#include <errno.h>
#include <internal/syscall.h>
#include <internal/arch/x86_64/syscall_list.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	/* TODO: Implement mmap(). */
	long rc = syscall(__NR_mmap, addr, length, prot, flags,
					  fd, offset);

	if (rc < 0) {
		errno = -rc;
		return MAP_FAILED;
	}

	return (void*)rc;
}

void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
{
	long rc = syscall(__NR_mremap, old_address, old_size,
					  new_size, flags);

	if (rc < 0) {
		errno = rc;
		return MAP_FAILED;
	}

	return (void*)rc;
}

int munmap(void *addr, size_t length)
{
	long rc = syscall(__NR_munmap, addr, length);

	if (rc < 0) {
		errno = rc;
		return -1;
	}

	return (int)rc;
}
