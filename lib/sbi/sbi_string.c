/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

/*
 * Simple libc functions. These are not optimized at all and might have some
 * bugs as well. Use any optimized routines from newlib or glibc if required.
 */

#include <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
ціле sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	для (; *a == *b && *a != '\0'; a++, b++)
		;

	повернути *a - *b;
}

ціле sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	для (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	якщо (!count)
		повернути 0;

	повернути *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	поки (*str != '\0') {
		ret++;
		str++;
	}

	повернути ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	поки (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	повернути ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	поки ((*dest++ = *src++) != '\0') {
	}
	повернути ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	поки (count) {
		якщо ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	повернути dest;
}

char *sbi_strchr(const char *s, ціле c)
{
	do {
		якщо (*s == (char)c)
			повернути (char *)s;
	} поки (*s++ != '\0');

	повернути NULL;
}

char *sbi_strrchr(const char *s, ціле c)
{
	const char *last = s + sbi_strlen(s);

	поки (last > s && *last != (char)c)
		last--;

	якщо (*last != (char)c)
		повернути NULL;
	інакше
		повернути (char *)last;
}
void *sbi_memset(void *s, ціле c, size_t count)
{
	char *temp = s;

	поки (count > 0) {
		count--;
		*temp++ = c;
	}

	повернути s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	поки (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	повернути dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	якщо (src == dest)
		повернути dest;

	якщо (dest < src) {
		поки (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} інакше {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		поки (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	повернути dest;
}

ціле sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	для (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	якщо (count > 0)
		повернути *(unsigned char *)temp1 - *(unsigned char *)temp2;
	інакше
		повернути 0;
}

void *sbi_memchr(const void *s, ціле c, size_t count)
{
	const unsigned char *temp = s;

	поки (count > 0) {
		якщо ((unsigned char)c == *temp++) {
			повернути (void *)(temp - 1);
		}
		count--;
	}

	повернути NULL;
}
