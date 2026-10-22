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

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t شمار)
{
	/* search first diff or end of string */
	لكل (; شمار > 0 && *a == *b && *a != '\0'; a++, b++, شمار--)
		;

	/* No difference till the end */
	إذا (!شمار)
		أرجع 0;

	أرجع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	بیعلامت دراز ret = 0;

	طالما (*str != '\0') {
		ret++;
		str++;
	}

	أرجع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t شمار)
{
	بیعلامت دراز ret = 0;

	طالما (*str != '\0' && ret < شمار) {
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

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t شمار)
{
	حرف *tmp = dest;

	طالما (شمار) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		شمار--;
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
	ثابت حرف *آخرین = s + sbi_strlen(s);

	طالما (آخرین > s && *آخرین != (حرف)c)
		آخرین--;

	إذا (*آخرین != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)آخرین;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t شمار)
{
	حرف *موقت = s;

	طالما (شمار > 0) {
		شمار--;
		*موقت++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t شمار)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (شمار > 0) {
		*temp1++ = *temp2++;
		شمار--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t شمار)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		أرجع dest;

	إذا (dest < src) {
		طالما (شمار > 0) {
			*temp1++ = *temp2++;
			شمار--;
		}
	} وإلا {
		temp1 = (حرف *)dest + شمار - 1;
		temp2 = (حرف *)src + شمار - 1;

		طالما (شمار > 0) {
			*temp1-- = *temp2--;
			شمار--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t شمار)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; شمار > 0 && (*temp1 == *temp2); شمار--) {
		temp1++;
		temp2++;
	}

	إذا (شمار > 0)
		أرجع *(بیعلامت حرف *)temp1 - *(بیعلامت حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t شمار)
{
	ثابت بیعلامت حرف *موقت = s;

	طالما (شمار > 0) {
		إذا ((بیعلامت حرف)c == *موقت++) {
			أرجع (فراغ *)(موقت - 1);
		}
		شمار--;
	}

	أرجع عدم;
}
