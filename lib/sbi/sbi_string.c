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
ចំនួនគត់ sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	សម្រាប់ (; *a == *b && *a != '\0'; a++, b++)
		;

	ត្រឡប់ *a - *b;
}

ចំនួនគត់ sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	សម្រាប់ (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ប្រសិនបើ (!count)
		ត្រឡប់ 0;

	ត្រឡប់ *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	ខណៈពេល (*str != '\0') {
		ret++;
		str++;
	}

	ត្រឡប់ ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	ខណៈពេល (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ត្រឡប់ ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	ខណៈពេល ((*dest++ = *src++) != '\0') {
	}
	ត្រឡប់ ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	ខណៈពេល (count) {
		ប្រសិនបើ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ត្រឡប់ dest;
}

var_na *sbi_strchr(const var_na *s, ចំនួនគត់ c)
{
	do {
		ប្រសិនបើ (*s == (var_na)c)
			ត្រឡប់ (var_na *)s;
	} ខណៈពេល (*s++ != '\0');

	ត្រឡប់ NULL;
}

var_na *sbi_strrchr(const var_na *s, ចំនួនគត់ c)
{
	const var_na *last = s + sbi_strlen(s);

	ខណៈពេល (last > s && *last != (var_na)c)
		last--;

	ប្រសិនបើ (*last != (var_na)c)
		ត្រឡប់ NULL;
	បើមិនដូច្នេះ
		ត្រឡប់ (var_na *)last;
}
void *sbi_memset(void *s, ចំនួនគត់ c, size_t count)
{
	var_na *temp = s;

	ខណៈពេល (count > 0) {
		count--;
		*temp++ = c;
	}

	ត្រឡប់ s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	ខណៈពេល (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ត្រឡប់ dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	ប្រសិនបើ (src == dest)
		ត្រឡប់ dest;

	ប្រសិនបើ (dest < src) {
		ខណៈពេល (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} បើមិនដូច្នេះ {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		ខណៈពេល (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ត្រឡប់ dest;
}

ចំនួនគត់ sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	សម្រាប់ (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ប្រសិនបើ (count > 0)
		ត្រឡប់ *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	បើមិនដូច្នេះ
		ត្រឡប់ 0;
}

void *sbi_memchr(const void *s, ចំនួនគត់ c, size_t count)
{
	const unsigned var_na *temp = s;

	ខណៈពេល (count > 0) {
		ប្រសិនបើ ((unsigned var_na)c == *temp++) {
			ត្រឡប់ (void *)(temp - 1);
		}
		count--;
	}

	ត្រឡប់ NULL;
}
