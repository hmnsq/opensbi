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
จำนวนเต็ม sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	สำหรับ (; *a == *b && *a != '\0'; a++, b++)
		;

	คืน *a - *b;
}

จำนวนเต็ม sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	สำหรับ (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ถ้า (!count)
		คืน 0;

	คืน *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	ขณะ (*str != '\0') {
		ret++;
		str++;
	}

	คืน ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	ขณะ (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	คืน ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	ขณะ ((*dest++ = *src++) != '\0') {
	}
	คืน ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	ขณะ (count) {
		ถ้า ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	คืน dest;
}

var_na *sbi_strchr(const var_na *s, จำนวนเต็ม c)
{
	do {
		ถ้า (*s == (var_na)c)
			คืน (var_na *)s;
	} ขณะ (*s++ != '\0');

	คืน NULL;
}

var_na *sbi_strrchr(const var_na *s, จำนวนเต็ม c)
{
	const var_na *last = s + sbi_strlen(s);

	ขณะ (last > s && *last != (var_na)c)
		last--;

	ถ้า (*last != (var_na)c)
		คืน NULL;
	มิฉะนั้น
		คืน (var_na *)last;
}
void *sbi_memset(void *s, จำนวนเต็ม c, size_t count)
{
	var_na *temp = s;

	ขณะ (count > 0) {
		count--;
		*temp++ = c;
	}

	คืน s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	ขณะ (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	คืน dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	ถ้า (src == dest)
		คืน dest;

	ถ้า (dest < src) {
		ขณะ (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} มิฉะนั้น {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		ขณะ (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	คืน dest;
}

จำนวนเต็ม sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	สำหรับ (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ถ้า (count > 0)
		คืน *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	มิฉะนั้น
		คืน 0;
}

void *sbi_memchr(const void *s, จำนวนเต็ม c, size_t count)
{
	const unsigned var_na *temp = s;

	ขณะ (count > 0) {
		ถ้า ((unsigned var_na)c == *temp++) {
			คืน (void *)(temp - 1);
		}
		count--;
	}

	คืน NULL;
}
