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
ຈຳນວນເຕັມ sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	ສຳລັບ (; *a == *b && *a != '\0'; a++, b++)
		;

	ສົ່ງຄືນ *a - *b;
}

ຈຳນວນເຕັມ sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	ສຳລັບ (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ຖ້າ (!count)
		ສົ່ງຄືນ 0;

	ສົ່ງຄືນ *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	ໃນຂະນະທີ່ (*str != '\0') {
		ret++;
		str++;
	}

	ສົ່ງຄືນ ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	ໃນຂະນະທີ່ (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ສົ່ງຄືນ ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	ໃນຂະນະທີ່ ((*dest++ = *src++) != '\0') {
	}
	ສົ່ງຄືນ ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	ໃນຂະນະທີ່ (count) {
		ຖ້າ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ສົ່ງຄືນ dest;
}

var_na *sbi_strchr(const var_na *s, ຈຳນວນເຕັມ c)
{
	do {
		ຖ້າ (*s == (var_na)c)
			ສົ່ງຄືນ (var_na *)s;
	} ໃນຂະນະທີ່ (*s++ != '\0');

	ສົ່ງຄືນ NULL;
}

var_na *sbi_strrchr(const var_na *s, ຈຳນວນເຕັມ c)
{
	const var_na *last = s + sbi_strlen(s);

	ໃນຂະນະທີ່ (last > s && *last != (var_na)c)
		last--;

	ຖ້າ (*last != (var_na)c)
		ສົ່ງຄືນ NULL;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ (var_na *)last;
}
void *sbi_memset(void *s, ຈຳນວນເຕັມ c, size_t count)
{
	var_na *temp = s;

	ໃນຂະນະທີ່ (count > 0) {
		count--;
		*temp++ = c;
	}

	ສົ່ງຄືນ s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	ໃນຂະນະທີ່ (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ສົ່ງຄືນ dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	ຖ້າ (src == dest)
		ສົ່ງຄືນ dest;

	ຖ້າ (dest < src) {
		ໃນຂະນະທີ່ (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} ບໍ່ດັ່ງນັ້ນ {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		ໃນຂະນະທີ່ (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ສົ່ງຄືນ dest;
}

ຈຳນວນເຕັມ sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	ສຳລັບ (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ຖ້າ (count > 0)
		ສົ່ງຄືນ *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ 0;
}

void *sbi_memchr(const void *s, ຈຳນວນເຕັມ c, size_t count)
{
	const unsigned var_na *temp = s;

	ໃນຂະນະທີ່ (count > 0) {
		ຖ້າ ((unsigned var_na)c == *temp++) {
			ສົ່ງຄືນ (void *)(temp - 1);
		}
		count--;
	}

	ສົ່ງຄືນ NULL;
}
