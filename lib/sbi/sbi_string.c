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
정수 sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	위해 (; *a == *b && *a != '\0'; a++, b++)
		;

	반환 *a - *b;
}

정수 sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	위해 (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	만약 (!count)
		반환 0;

	반환 *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	동안 (*str != '\0') {
		ret++;
		str++;
	}

	반환 ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	동안 (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	반환 ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	동안 ((*dest++ = *src++) != '\0') {
	}
	반환 ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	동안 (count) {
		만약 ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	반환 dest;
}

char *sbi_strchr(const char *s, 정수 c)
{
	do {
		만약 (*s == (char)c)
			반환 (char *)s;
	} 동안 (*s++ != '\0');

	반환 NULL;
}

char *sbi_strrchr(const char *s, 정수 c)
{
	const char *last = s + sbi_strlen(s);

	동안 (last > s && *last != (char)c)
		last--;

	만약 (*last != (char)c)
		반환 NULL;
	아니면
		반환 (char *)last;
}
void *sbi_memset(void *s, 정수 c, size_t count)
{
	char *temp = s;

	동안 (count > 0) {
		count--;
		*temp++ = c;
	}

	반환 s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	동안 (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	반환 dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	만약 (src == dest)
		반환 dest;

	만약 (dest < src) {
		동안 (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} 아니면 {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		동안 (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	반환 dest;
}

정수 sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	위해 (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	만약 (count > 0)
		반환 *(unsigned char *)temp1 - *(unsigned char *)temp2;
	아니면
		반환 0;
}

void *sbi_memchr(const void *s, 정수 c, size_t count)
{
	const unsigned char *temp = s;

	동안 (count > 0) {
		만약 ((unsigned char)c == *temp++) {
			반환 (void *)(temp - 1);
		}
		count--;
	}

	반환 NULL;
}
