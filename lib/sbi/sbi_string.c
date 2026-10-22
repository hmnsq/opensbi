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
පූර්ණ_සංඛ්‍යා sbi_strcmp(const var_na *a, const var_na *b)
{
	/* search first diff or end of string */
	සඳහා (; *a == *b && *a != '\0'; a++, b++)
		;

	ආපසු *a - *b;
}

පූර්ණ_සංඛ්‍යා sbi_strncmp(const var_na *a, const var_na *b, size_t count)
{
	/* search first diff or end of string */
	සඳහා (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	නම් (!count)
		ආපසු 0;

	ආපසු *a - *b;
}

size_t sbi_strlen(const var_na *str)
{
	unsigned long ret = 0;

	අතරතුර (*str != '\0') {
		ret++;
		str++;
	}

	ආපසු ret;
}

size_t sbi_strnlen(const var_na *str, size_t count)
{
	unsigned long ret = 0;

	අතරතුර (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ආපසු ret;
}

var_na *sbi_strcpy(var_na *dest, const var_na *src)
{
	var_na *ret = dest;

	අතරතුර ((*dest++ = *src++) != '\0') {
	}
	ආපසු ret;
}

var_na *sbi_strncpy(var_na *dest, const var_na *src, size_t count)
{
	var_na *tmp = dest;

	අතරතුර (count) {
		නම් ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ආපසු dest;
}

var_na *sbi_strchr(const var_na *s, පූර්ණ_සංඛ්‍යා c)
{
	do {
		නම් (*s == (var_na)c)
			ආපසු (var_na *)s;
	} අතරතුර (*s++ != '\0');

	ආපසු NULL;
}

var_na *sbi_strrchr(const var_na *s, පූර්ණ_සංඛ්‍යා c)
{
	const var_na *last = s + sbi_strlen(s);

	අතරතුර (last > s && *last != (var_na)c)
		last--;

	නම් (*last != (var_na)c)
		ආපසු NULL;
	නැතහොත්
		ආපසු (var_na *)last;
}
void *sbi_memset(void *s, පූර්ණ_සංඛ්‍යා c, size_t count)
{
	var_na *temp = s;

	අතරතුර (count > 0) {
		count--;
		*temp++ = c;
	}

	ආපසු s;
}

void *sbi_memcpy(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = dest;
	const var_na *temp2 = src;

	අතරතුර (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ආපසු dest;
}

void *sbi_memmove(void *dest, const void *src, size_t count)
{
	var_na *temp1	  = (var_na *)dest;
	const var_na *temp2 = (var_na *)src;

	නම් (src == dest)
		ආපසු dest;

	නම් (dest < src) {
		අතරතුර (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} නැතහොත් {
		temp1 = (var_na *)dest + count - 1;
		temp2 = (var_na *)src + count - 1;

		අතරතුර (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ආපසු dest;
}

පූර්ණ_සංඛ්‍යා sbi_memcmp(const void *s1, const void *s2, size_t count)
{
	const var_na *temp1 = s1;
	const var_na *temp2 = s2;

	සඳහා (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	නම් (count > 0)
		ආපසු *(unsigned var_na *)temp1 - *(unsigned var_na *)temp2;
	නැතහොත්
		ආපසු 0;
}

void *sbi_memchr(const void *s, පූර්ණ_සංඛ්‍යා c, size_t count)
{
	const unsigned var_na *temp = s;

	අතරතුර (count > 0) {
		නම් ((unsigned var_na)c == *temp++) {
			ආපසු (void *)(temp - 1);
		}
		count--;
	}

	ආපසු NULL;
}
