// SPDX-License-Identifier: BSD-3-Clause

#include <internal/mm/mem_list.h>
#include <internal/types.h>
#include <internal/essentials.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int is_list_initialised = 0;

void *malloc(size_t size)
{
	if (size == 0) {
		errno = EINVAL;
		return NULL;
	}

	if (!is_list_initialised) {
		mem_list_init();
		is_list_initialised = 1;
	}

	void* data = mmap(NULL, size, PROT_READ | PROT_WRITE,
					  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (data == MAP_FAILED) {
		errno = ENOMEM;
		return NULL;
	}

	mem_list_add(data, size);

	return data;
}

void *calloc(size_t nmemb, size_t size)
{
	size_t num_bytes = nmemb * size;
	void* data = (char*)malloc(num_bytes);

	if (data == NULL) return NULL;

	memset(data, 0, num_bytes);

	return data;
}

void free(void *ptr)
{
	struct mem_list* ptr_pos = mem_list_find(ptr);

	if (ptr_pos == NULL) return;

	munmap(ptr_pos->start, ptr_pos->len);
	mem_list_del(ptr_pos->start);
}

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL) return malloc(size);
	if (size == 0) return NULL;

	struct mem_list *current = mem_list_find(ptr);

	if (current == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	size_t old_size = current->len;

	void *new_ptr = mremap(ptr, old_size, size, 1);

	if (new_ptr == MAP_FAILED) return NULL;

	mem_list_del(ptr);
	mem_list_add(new_ptr, size);

	return new_ptr;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	size_t total_size = nmemb * size;

	return realloc(ptr, total_size);
}
