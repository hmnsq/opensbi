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
puur_naa_nka sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	for (; *a == *b && *a != '\0'; a++, b++)
		;

	ᯀᯮᯞ *a - *b;
}

puur_naa_nka sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	for (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ᯀᯮᯝ (!count)
		ᯀᯮᯞ 0;

	ᯀᯮᯞ *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	ᯀᯬᯝ (*str != '\0') {
		ret++;
		str++;
	}

	ᯀᯮᯞ ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	ᯀᯬᯝ (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ᯀᯮᯞ ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	ᯀᯬᯝ ((*dest++ = *src++) != '\0') {
	}
	ᯀᯮᯞ ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	ᯀᯬᯝ (count) {
		ᯀᯮᯝ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ᯀᯮᯞ dest;
}

var_na *sbi_strchr(const var_na *s, puur_naa_nka c)
{
	do {
		ᯀᯮᯝ (*s == (var_na)c)
			ᯀᯮᯞ (var_na *)s;
	} ᯀᯬᯝ (*s++ != '\0');

	ᯀᯮᯞ NULL;
}

var_na *sbi_strrchr(const var_na *s, puur_naa_nka c)
{
	const var_na *last = s + sbi_strlen(s);

	ᯀᯬᯝ (last > s && *last != (var_na)c)
		last--;

	ᯀᯮᯝ (*last != (var_na)c)
		ᯀᯮᯞ NULL;
	ᯀᯮᯝᯂᯪ
		ᯀᯮᯞ (var_na *)last;
}
void *sbi_memset(void *s, puur_naa_nka c, size_t count)
{
	var_na *temp = s;

	ᯀᯬᯝ (count > 0) {
		count--;
		*temp++ = c;
	}

	ᯀᯮᯞ s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	ᯀᯬᯝ (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ᯀᯮᯞ dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	ᯀᯮᯝ (src == dest)
		ᯀᯮᯞ dest;

	ᯀᯮᯝ (dest < src) {
		ᯀᯬᯝ (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} ᯀᯮᯝᯂᯪ {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		ᯀᯬᯝ (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ᯀᯮᯞ dest;
}

puur_naa_nka sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	for (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ᯀᯮᯝ (count > 0)
		ᯀᯮᯞ *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	ᯀᯮᯝᯂᯪ
		ᯀᯮᯞ 0;
}

void *sbi_memchr(const void *s, puur_naa_nka c, size_t count)
{
	const unsigned var_na *temp = s;

	ᯀᯬᯝ (count > 0) {
		ᯀᯮᯝ ((unsigned var_na)c == *temp++) {
			ᯀᯮᯞ (void *)(temp - 1);
		}
		count--;
	}

	ᯀᯮᯞ NULL;
}
