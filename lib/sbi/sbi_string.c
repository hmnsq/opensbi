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
цяло sbi_strcmp(const char *a, const char *b)
{
	/* search first diff or end of string */
	за (; *a == *b && *a != '\0'; a++, b++)
		;

	върни *a - *b;
}

цяло sbi_strncmp(const char *a, const char *b, size_t count)
{
	/* search first diff or end of string */
	за (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ако (!count)
		върни 0;

	върни *a - *b;
}

size_t sbi_strlen(const char *str)
{
	unsigned long ret = 0;

	докато (*str != '\0') {
		ret++;
		str++;
	}

	върни ret;
}

size_t sbi_strnlen(const char *str, size_t count)
{
	unsigned long ret = 0;

	докато (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	върни ret;
}

char *sbi_strcpy(char *dest, const char *src)
{
	char *ret = dest;

	докато ((*dest++ = *src++) != '\0') {
	}
	върни ret;
}

char *sbi_strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	докато (count) {
		ако ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	върни dest;
}

char *sbi_strchr(const char *s, цяло c)
{
	do {
		ако (*s == (char)c)
			върни (char *)s;
	} докато (*s++ != '\0');

	върни NULL;
}

char *sbi_strrchr(const char *s, цяло c)
{
	const char *last = s + sbi_strlen(s);

	докато (last > s && *last != (char)c)
		last--;

	ако (*last != (char)c)
		върни NULL;
	иначе
		върни (char *)last;
}
void *sbi_memset(void *s, цяло c, size_t count)
{
	char *temp = s;

	докато (count > 0) {
		count--;
		*temp++ = c;
	}

	върни s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	char *temp1	  = dest;
	const char *temp2 = src;

	докато (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	върни dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	char *temp1	  = (char *)dest;
	const char *temp2 = (char *)src;

	ако (src == dest)
		върни dest;

	ако (dest < src) {
		докато (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} иначе {
		temp1 = (char *)dest + count - 1;
		temp2 = (char *)src + count - 1;

		докато (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	върни dest;
}

цяло sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const char *temp1 = s1;
	const char *temp2 = s2;

	за (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ако (count > 0)
		върни *(unsigned char *)temp1 - *(unsigned char *)temp2;
	иначе
		върни 0;
}

void *sbi_memchr(const void *s, цяло c, size_t count)
{
	const unsigned char *temp = s;

	докато (count > 0) {
		ако ((unsigned char)c == *temp++) {
			върни (void *)(temp - 1);
		}
		count--;
	}

	върни NULL;
}
