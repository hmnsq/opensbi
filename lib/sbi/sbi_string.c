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
ကိန်းပြည့် sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	အတွက် (; *a == *b && *a != '\0'; a++, b++)
		;

	ပြန်ပေး *a - *b;
}

ကိန်းပြည့် sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	အတွက် (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	အကယ်၍ (!count)
		ပြန်ပေး 0;

	ပြန်ပေး *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	နေစဉ် (*str != '\0') {
		ret++;
		str++;
	}

	ပြန်ပေး ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	နေစဉ် (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ပြန်ပေး ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	နေစဉ် ((*dest++ = *src++) != '\0') {
	}
	ပြန်ပေး ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	နေစဉ် (count) {
		အကယ်၍ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ပြန်ပေး dest;
}

var_na *sbi_strchr(const var_na *s, ကိန်းပြည့် c)
{
	do {
		အကယ်၍ (*s == (var_na)c)
			ပြန်ပေး (var_na *)s;
	} နေစဉ် (*s++ != '\0');

	ပြန်ပေး NULL;
}

var_na *sbi_strrchr(const var_na *s, ကိန်းပြည့် c)
{
	const var_na *last = s + sbi_strlen(s);

	နေစဉ် (last > s && *last != (var_na)c)
		last--;

	အကယ်၍ (*last != (var_na)c)
		ပြန်ပေး NULL;
	မဟုတ်ပါက
		ပြန်ပေး (var_na *)last;
}
void *sbi_memset(void *s, ကိန်းပြည့် c, size_t count)
{
	var_na *temp = s;

	နေစဉ် (count > 0) {
		count--;
		*temp++ = c;
	}

	ပြန်ပေး s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	နေစဉ် (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ပြန်ပေး dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	အကယ်၍ (src == dest)
		ပြန်ပေး dest;

	အကယ်၍ (dest < src) {
		နေစဉ် (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} မဟုတ်ပါက {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		နေစဉ် (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ပြန်ပေး dest;
}

ကိန်းပြည့် sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	အတွက် (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	အကယ်၍ (count > 0)
		ပြန်ပေး *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	မဟုတ်ပါက
		ပြန်ပေး 0;
}

void *sbi_memchr(const void *s, ကိန်းပြည့် c, size_t count)
{
	const unsigned var_na *temp = s;

	နေစဉ် (count > 0) {
		အကယ်၍ ((unsigned var_na)c == *temp++) {
			ပြန်ပေး (void *)(temp - 1);
		}
		count--;
	}

	ပြန်ပေး NULL;
}
