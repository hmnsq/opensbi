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
цео sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	за (; *a == *b && *a != '\0'; a++, b++)
		;

	врати *a - *b;
}

цео sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	за (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ако (!count)
		врати 0;

	врати *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	док (*str != '\0') {
		ret++;
		str++;
	}

	врати ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	док (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	врати ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	док ((*dest++ = *src++) != '\0') {
	}
	врати ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	док (count) {
		ако ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	врати dest;
}

char *sbi_strchr(const char *s, цео c)
{
	do {
		ако (*s == (char)c)
			врати (char *)s;
	} док (*s++ != '\0');

	врати NULL;
}

char *sbi_strrchr(const char *s, цео c)
{
	const char *last = s + sbi_strlen(s);

	док (last > s && *last != (char)c)
		last--;

	ако (*last != (char)c)
		врати NULL;
	инако
		врати (char *)last;
}
void *sbi_memset(void *s, цео c, size_t count)
{
	char *temp = s;

	док (count > 0) {
		count--;
		*temp++ = c;
	}

	врати s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	док (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	врати dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	ако (src == dest)
		врати dest;

	ако (dest < src) {
		док (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} инако {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		док (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	врати dest;
}

цео sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	за (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ако (count > 0)
		врати *(unsigned char *)temp1 - *(unsigned char *)temp2;
	инако
		врати 0;
}

void *sbi_memchr(const void *s, цео c, size_t count)
{
	const unsigned char *temp = s;

	док (count > 0) {
		ако ((unsigned char)c == *temp++) {
			врати (void *)(temp - 1);
		}
		count--;
	}

	врати NULL;
}
