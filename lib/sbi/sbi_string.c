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

	ᮝᮜᮤ *a - *b;
}

puur_naa_nka sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	for (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ᮚᮦᮔ᮪ (!count)
		ᮝᮜᮤ 0;

	ᮝᮜᮤ *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	ᮞᮨᮓᮨᮜ (*str != '\0') {
		ret++;
		str++;
	}

	ᮝᮜᮤ ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	ᮞᮨᮓᮨᮜ (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ᮝᮜᮤ ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	ᮞᮨᮓᮨᮜ ((*dest++ = *src++) != '\0') {
	}
	ᮝᮜᮤ ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	ᮞᮨᮓᮨᮜ (count) {
		ᮚᮦᮔ᮪ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ᮝᮜᮤ dest;
}

var_na *sbi_strchr(const var_na *s, puur_naa_nka c)
{
	do {
		ᮚᮦᮔ᮪ (*s == (var_na)c)
			ᮝᮜᮤ (var_na *)s;
	} ᮞᮨᮓᮨᮜ (*s++ != '\0');

	ᮝᮜᮤ NULL;
}

var_na *sbi_strrchr(const var_na *s, puur_naa_nka c)
{
	const var_na *last = s + sbi_strlen(s);

	ᮞᮨᮓᮨᮜ (last > s && *last != (var_na)c)
		last--;

	ᮚᮦᮔ᮪ (*last != (var_na)c)
		ᮝᮜᮤ NULL;
	ᮜᮤᮚ
		ᮝᮜᮤ (var_na *)last;
}
void *sbi_memset(void *s, puur_naa_nka c, size_t count)
{
	var_na *temp = s;

	ᮞᮨᮓᮨᮜ (count > 0) {
		count--;
		*temp++ = c;
	}

	ᮝᮜᮤ s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	ᮞᮨᮓᮨᮜ (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ᮝᮜᮤ dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	ᮚᮦᮔ᮪ (src == dest)
		ᮝᮜᮤ dest;

	ᮚᮦᮔ᮪ (dest < src) {
		ᮞᮨᮓᮨᮜ (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} ᮜᮤᮚ {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		ᮞᮨᮓᮨᮜ (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ᮝᮜᮤ dest;
}

puur_naa_nka sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	for (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ᮚᮦᮔ᮪ (count > 0)
		ᮝᮜᮤ *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	ᮜᮤᮚ
		ᮝᮜᮤ 0;
}

void *sbi_memchr(const void *s, puur_naa_nka c, size_t count)
{
	const unsigned var_na *temp = s;

	ᮞᮨᮓᮨᮜ (count > 0) {
		ᮚᮦᮔ᮪ ((unsigned var_na)c == *temp++) {
			ᮝᮜᮤ (void *)(temp - 1);
		}
		count--;
	}

	ᮝᮜᮤ NULL;
}
