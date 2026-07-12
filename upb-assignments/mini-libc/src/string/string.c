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
	size_t dest_len = strlen(destination);
	size_t i;

	for (i = 0; source[i] != '\0'; i++)
		destination[dest_len + i] = source[i];
	destination[i] = '\0';

	return destination;
}

char *strncat(char *destination, const char *source, size_t len)
{
	size_t dest_len = strlen(destination);
	size_t i;

	for (i = 0; source[i] != '\0' && i < len; i++)
		destination[dest_len + i] = source[i];
	destination[i] = '\0';
	
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
	size_t i;

	for (i = 0; str1[i] != '\0' && str2[i] != '\0' && i < len; i++)
		if (str1[i] != str2[i]) return (str1[i] - str2[i]);

	return (str1[i] - str2[i]);
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
		if (str[i] == c) return &str[i];

	if (str[i] == c) return &str[i];

	return NULL;
}

char *strrchr(const char *str, int c)
{
	char* last_occ = NULL;
	size_t i;

	for (i = 0; str[i] != '\0'; i++)
		if (str[i] == c) last_occ = &str[i];

	if (str[i] == c) last_occ = &str[i];

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
	/* TODO: Implement strrstr(). */
	return NULL;
}

void *memcpy(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memcpy(). */
	return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
	/* TODO: Implement memmove(). */
	return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	/* TODO: Implement memcmp(). */
	return -1;
}

void *memset(void *source, int value, size_t num)
{
	/* TODO: Implement memset(). */
	return source;
}
