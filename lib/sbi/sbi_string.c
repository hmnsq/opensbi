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

	أرجع *a - *b;
}

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t ژماردن)
{
	/* search first diff or end of string */
	لكل (; ژماردن > 0 && *a == *b && *a != '\0'; a++, b++, ژماردن--)
		;

	/* No difference till the end */
	إذا (!ژماردن)
		أرجع 0;

	أرجع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	unsigned long ret = 0;

	طالما (*str != '\0') {
		ret++;
		str++;
	}

	أرجع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t ژماردن)
{
	unsigned long ret = 0;

	طالما (*str != '\0' && ret < ژماردن) {
		ret++;
		str++;
	}

	أرجع ret;
}

حرف *sbi_strcpy(حرف *dest, ثابت حرف *src)
{
	حرف *ret = dest;

	طالما ((*dest++ = *src++) != '\0') {
	}
	أرجع ret;
}

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t ژماردن)
{
	حرف *tmp = dest;

	طالما (ژماردن) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		ژماردن--;
	}
	أرجع dest;
}

حرف *sbi_strchr(ثابت حرف *s, صحيح c)
{
	افعل {
		إذا (*s == (حرف)c)
			أرجع (حرف *)s;
	} طالما (*s++ != '\0');

	أرجع عدم;
}

حرف *sbi_strrchr(ثابت حرف *s, صحيح c)
{
	ثابت حرف *last = s + sbi_strlen(s);

	طالما (last > s && *last != (حرف)c)
		last--;

	إذا (*last != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)last;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t ژماردن)
{
	حرف *temp = s;

	طالما (ژماردن > 0) {
		ژماردن--;
		*temp++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t ژماردن)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (ژماردن > 0) {
		*temp1++ = *temp2++;
		ژماردن--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t ژماردن)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		أرجع dest;

	إذا (dest < src) {
		طالما (ژماردن > 0) {
			*temp1++ = *temp2++;
			ژماردن--;
		}
	} وإلا {
		temp1 = (حرف *)dest + ژماردن - 1;
		temp2 = (حرف *)src + ژماردن - 1;

		طالما (ژماردن > 0) {
			*temp1-- = *temp2--;
			ژماردن--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t ژماردن)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; ژماردن > 0 && (*temp1 == *temp2); ژماردن--) {
		temp1++;
		temp2++;
	}

	إذا (ژماردن > 0)
		أرجع *(unsigned حرف *)temp1 - *(unsigned حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t ژماردن)
{
	ثابت unsigned حرف *temp = s;

	طالما (ژماردن > 0) {
		إذا ((unsigned حرف)c == *temp++) {
			أرجع (فراغ *)(temp - 1);
		}
		ژماردن--;
	}

	أرجع عدم;
}
