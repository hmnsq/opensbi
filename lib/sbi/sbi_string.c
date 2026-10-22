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
số_nguyên sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	cho (; *a == *b && *a != '\0'; a++, b++)
		;

	trả_về *a - *b;
}

số_nguyên sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	cho (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	nếu (!count)
		trả_về 0;

	trả_về *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	trong_khi (*str != '\0') {
		ret++;
		str++;
	}

	trả_về ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	trong_khi (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	trả_về ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	trong_khi ((*dest++ = *src++) != '\0') {
	}
	trả_về ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	trong_khi (count) {
		nếu ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	trả_về dest;
}

char *sbi_strchr(const char *s, số_nguyên c)
{
	do {
		nếu (*s == (char)c)
			trả_về (char *)s;
	} trong_khi (*s++ != '\0');

	trả_về NULL;
}

char *sbi_strrchr(const char *s, số_nguyên c)
{
	const char *last = s + sbi_strlen(s);

	trong_khi (last > s && *last != (char)c)
		last--;

	nếu (*last != (char)c)
		trả_về NULL;
	không_thì
		trả_về (char *)last;
}
void *sbi_memset(void *s, số_nguyên c, size_t count)
{
	char *temp = s;

	trong_khi (count > 0) {
		count--;
		*temp++ = c;
	}

	trả_về s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	trong_khi (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	trả_về dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	nếu (src == dest)
		trả_về dest;

	nếu (dest < src) {
		trong_khi (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} không_thì {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		trong_khi (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	trả_về dest;
}

số_nguyên sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	cho (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	nếu (count > 0)
		trả_về *(unsigned char *)temp1 - *(unsigned char *)temp2;
	không_thì
		trả_về 0;
}

void *sbi_memchr(const void *s, số_nguyên c, size_t count)
{
	const unsigned char *temp = s;

	trong_khi (count > 0) {
		nếu ((unsigned char)c == *temp++) {
			trả_về (void *)(temp - 1);
		}
		count--;
	}

	trả_về NULL;
}
