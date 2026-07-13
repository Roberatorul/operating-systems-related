// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

char *strcpy(char *destination, const char *source)
{
	size_t i;

	for (i = 0; source[i] != '\0'; i++)
		destination[i] = source[i];
	destination[i] = '\0';

	return destination;
}

char *strncpy(char *destination, const char *source, size_t len)
{
	size_t i;

	for (i = 0; source[i] != '\0' && i < len; i++)
		destination[i] = source[i];

	while (i < len)
		destination[i++] = '\0';

	return destination;
}

char *strcat(char *destination, const char *source)
{
	size_t i;
	size_t j;

	for (i = 0; destination[i] != '\0'; i++) { }

	for (j = 0; source[j] != '\0'; j++)
		destination[i + j] = source[j];
	destination[i + j] = '\0';

	return destination;
}


char *strncat(char *destination, const char *source, size_t len)
{
	size_t i;
	size_t j;

	for (i = 0; destination[i] != '\0'; i++) { }

	for (j = 0; source[j] != '\0' && j < len; j++)
		destination[i + j] = source[j];
	destination[i + j] = '\0';

	return destination;
}


int strcmp(const char *str1, const char *str2)
{
	size_t i;

	for (i = 0; str1[i] != '\0' && str2[i] != '\0'; i++)
		if (str1[i] != str2[i]) return (str1[i] - str2[i]);

	return (str1[i] - str2[i]);
}

int strncmp(const char *str1, const char *str2, size_t len)
{
	if (len == 0) return 0;

	size_t i;

	for (i = 0; str1[i] != '\0' && str2[i] != '\0' && i < len; i++)
		if (str1[i] != str2[i]) return (str1[i] - str2[i]);

	return 0;
}

size_t strlen(const char *str)
{
	size_t i = 0;

	for (; *str != '\0'; str++, i++)
		;

	return i;
}

char *strchr(const char *str, int c)
{
	size_t i;

	for (i = 0; str[i] != '\0'; i++)
		if (str[i] == c) return (char*)&str[i];

	if (str[i] == c) return (char*	)&str[i];

	return NULL;
}

char *strrchr(const char *str, int c)
{
	char* last_occ = NULL;
	size_t i;

	for (i = 0; str[i] != '\0'; i++)
		if (str[i] == c) last_occ = (char*)&str[i];

	if (str[i] == c) last_occ = (char*)&str[i];

	return last_occ;
}

char *strstr(const char *haystack, const char *needle)
{
	int is_valid;

	for (size_t i = 0; haystack[i] != '\0'; i++) {
		is_valid = 1;
		for (size_t j = 0; needle[j] != '\0'; j++) {
			if (haystack[i + j] != needle[j]) {
				is_valid = 0;
				break;
			}
		}

		if (is_valid) return (char*)&haystack[i];
	}


	return NULL;
}

char *strrstr(const char *haystack, const char *needle)
{
	int is_valid;
	size_t i, j;
	char* last_occ = NULL;

	for (i = 0; haystack[i] != '\0'; i++) {
		is_valid = 1;
		for (j = 0; needle[j] != '\0'; j++) {
			if (haystack[i + j] != needle[j]) {
				is_valid = 0;
				break;
			}
		}

		if (is_valid) {
			last_occ = (char*)&haystack[i];
			i += j - 1;
		}
	}

	return last_occ;
}

void *memcpy(void *destination, const void *source, size_t num)
{
	char* dest_ptr = (char*)destination;
	char* src_ptr = (char*)source;

	for (size_t i = 0; i < num; i++)
		dest_ptr[i] = src_ptr[i];

	return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
    char* dest_ptr = (char*)destination;
    char* src_ptr = (char*)source;

    if (dest_ptr < src_ptr) {
        memcpy(destination, source, num);
        return destination;
    }

    for (int i = num - 1; i >= 0; i--)
        dest_ptr[i] = src_ptr[i];

    return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	size_t i;
	char* p1 = (char*)ptr1;
	char* p2 = (char*)ptr2;

	for (i = 0; i < num && p1[i] == p2[i]; i++) { }

	if (i == num) return 0;

	return (p1[i] - p2[i]);
}

void *memset(void *source, int value, size_t num)
{
	char* src_ptr = (char*)source;

	for (size_t i = 0; i < num; i++)
		src_ptr[i] = (unsigned char)value;

	return source;
}
