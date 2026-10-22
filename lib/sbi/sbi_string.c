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
целое sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	для (; *a == *b && *a != '\0'; a++, b++)
		;

	вернуть *a - *b;
}

целое sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	для (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	если (!count)
		вернуть 0;

	вернуть *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	пока (*str != '\0') {
		ret++;
		str++;
	}

	вернуть ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	пока (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	вернуть ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	пока ((*dest++ = *src++) != '\0') {
	}
	вернуть ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	пока (count) {
		если ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	вернуть dest;
}

char *sbi_strchr(const char *s, целое c)
{
	do {
		если (*s == (char)c)
			вернуть (char *)s;
	} пока (*s++ != '\0');

	вернуть NULL;
}

char *sbi_strrchr(const char *s, целое c)
{
	const char *last = s + sbi_strlen(s);

	пока (last > s && *last != (char)c)
		last--;

	если (*last != (char)c)
		вернуть NULL;
	иначе
		вернуть (char *)last;
}
void *sbi_memset(void *s, целое c, size_t count)
{
	char *temp = s;

	пока (count > 0) {
		count--;
		*temp++ = c;
	}

	вернуть s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	пока (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	вернуть dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	если (src == dest)
		вернуть dest;

	если (dest < src) {
		пока (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} иначе {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		пока (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	вернуть dest;
}

целое sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	для (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	если (count > 0)
		вернуть *(unsigned char *)temp1 - *(unsigned char *)temp2;
	иначе
		вернуть 0;
}

void *sbi_memchr(const void *s, целое c, size_t count)
{
	const unsigned char *temp = s;

	пока (count > 0) {
		если ((unsigned char)c == *temp++) {
			вернуть (void *)(temp - 1);
		}
		count--;
	}

	вернуть NULL;
}
