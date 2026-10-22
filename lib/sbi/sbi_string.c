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
ཧྲིལ sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	དོན་ལུ (; *a == *b && *a != '\0'; a++, b++)
		;

	ལོག *a - *b;
}

ཧྲིལ sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	དོན་ལུ (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	གལ་སྲིད (!count)
		ལོག 0;

	ལོག *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	སྐབས (*str != '\0') {
		ret++;
		str++;
	}

	ལོག ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	སྐབས (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ལོག ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	སྐབས ((*dest++ = *src++) != '\0') {
	}
	ལོག ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	སྐབས (count) {
		གལ་སྲིད ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ལོག dest;
}

var_na *sbi_strchr(const var_na *s, ཧྲིལ c)
{
	do {
		གལ་སྲིད (*s == (var_na)c)
			ལོག (var_na *)s;
	} སྐབས (*s++ != '\0');

	ལོག NULL;
}

var_na *sbi_strrchr(const var_na *s, ཧྲིལ c)
{
	const var_na *last = s + sbi_strlen(s);

	སྐབས (last > s && *last != (var_na)c)
		last--;

	གལ་སྲིད (*last != (var_na)c)
		ལོག NULL;
	དེ་མིན
		ལོག (var_na *)last;
}
void *sbi_memset(void *s, ཧྲིལ c, size_t count)
{
	var_na *temp = s;

	སྐབས (count > 0) {
		count--;
		*temp++ = c;
	}

	ལོག s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	སྐབས (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ལོག dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	གལ་སྲིད (src == dest)
		ལོག dest;

	གལ་སྲིད (dest < src) {
		སྐབས (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} དེ་མིན {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		སྐབས (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ལོག dest;
}

ཧྲིལ sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	དོན་ལུ (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	གལ་སྲིད (count > 0)
		ལོག *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	དེ་མིན
		ལོག 0;
}

void *sbi_memchr(const void *s, ཧྲིལ c, size_t count)
{
	const unsigned var_na *temp = s;

	སྐབས (count > 0) {
		གལ་སྲིད ((unsigned var_na)c == *temp++) {
			ལོག (void *)(temp - 1);
		}
		count--;
	}

	ལོག NULL;
}
