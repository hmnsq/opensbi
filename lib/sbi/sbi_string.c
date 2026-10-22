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

#ضمن <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
صحيح sbi_strcmp(ثابت حرف *a, ثابت حرف *b)
{
	/* search first diff or end of string */
	لكل (; *a == *b && *a != '\0'; a++, b++)
		;

	ܦܢܐ *a - *b;
}

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t count)
{
	/* search first diff or end of string */
	لكل (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	ܐܢ (!count)
		ܦܢܐ 0;

	ܦܢܐ *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	unsigned long ret = 0;

	ܟܡܐ (*str != '\0') {
		ret++;
		str++;
	}

	ܦܢܐ ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t count)
{
	unsigned long ret = 0;

	ܟܡܐ (*str != '\0' && ret < count) {
		ret++;
		str++;
	}

	ܦܢܐ ret;
}

حرف *sbi_strcpy(حرف *dest, ثابت حرف *src)
{
	حرف *ret = dest;

	ܟܡܐ ((*dest++ = *src++) != '\0') {
	}
	ܦܢܐ ret;
}

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t count)
{
	حرف *tmp = dest;

	ܟܡܐ (count) {
		ܐܢ ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	ܦܢܐ dest;
}

حرف *sbi_strchr(ثابت حرف *s, صحيح c)
{
	ܥܒܕ {
		ܐܢ (*s == (حرف)c)
			ܦܢܐ (حرف *)s;
	} ܟܡܐ (*s++ != '\0');

	ܦܢܐ عدم;
}

حرف *sbi_strrchr(ثابت حرف *s, صحيح c)
{
	ثابت حرف *last = s + sbi_strlen(s);

	ܟܡܐ (last > s && *last != (حرف)c)
		last--;

	ܐܢ (*last != (حرف)c)
		ܦܢܐ عدم;
	ܐܚܪܢܐ
		ܦܢܐ (حرف *)last;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t count)
{
	حرف *temp = s;

	ܟܡܐ (count > 0) {
		count--;
		*temp++ = c;
	}

	ܦܢܐ s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t count)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	ܟܡܐ (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	ܦܢܐ dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t count)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	ܐܢ (src == dest)
		ܦܢܐ dest;

	ܐܢ (dest < src) {
		ܟܡܐ (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} ܐܚܪܢܐ {
		temp1 = (حرف *)dest + count - 1;
		temp2 = (حرف *)src + count - 1;

		ܟܡܐ (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	ܦܢܐ dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t count)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	ܐܢ (count > 0)
		ܦܢܐ *(unsigned حرف *)temp1 - *(unsigned حرف *)temp2;
	ܐܚܪܢܐ
		ܦܢܐ 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t count)
{
	ثابت unsigned حرف *temp = s;

	ܟܡܐ (count > 0) {
		ܐܢ ((unsigned حرف)c == *temp++) {
			ܦܢܐ (فراغ *)(temp - 1);
		}
		count--;
	}

	ܦܢܐ عدم;
}
