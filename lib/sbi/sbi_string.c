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

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t شمېر)
{
	/* search first diff or end of string */
	لكل (; شمېر > 0 && *a == *b && *a != '\0'; a++, b++, شمېر--)
		;

	/* No difference till the end */
	إذا (!شمېر)
		أرجع 0;

	أرجع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	بینښه اوږد ret = 0;

	طالما (*str != '\0') {
		ret++;
		str++;
	}

	أرجع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t شمېر)
{
	بینښه اوږد ret = 0;

	طالما (*str != '\0' && ret < شمېر) {
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

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t شمېر)
{
	حرف *tmp = dest;

	طالما (شمېر) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		شمېر--;
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
	ثابت حرف *وروستی = s + sbi_strlen(s);

	طالما (وروستی > s && *وروستی != (حرف)c)
		وروستی--;

	إذا (*وروستی != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)وروستی;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t شمېر)
{
	حرف *لنډمهاله = s;

	طالما (شمېر > 0) {
		شمېر--;
		*لنډمهاله++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t شمېر)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (شمېر > 0) {
		*temp1++ = *temp2++;
		شمېر--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t شمېر)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		أرجع dest;

	إذا (dest < src) {
		طالما (شمېر > 0) {
			*temp1++ = *temp2++;
			شمېر--;
		}
	} وإلا {
		temp1 = (حرف *)dest + شمېر - 1;
		temp2 = (حرف *)src + شمېر - 1;

		طالما (شمېر > 0) {
			*temp1-- = *temp2--;
			شمېر--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t شمېر)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; شمېر > 0 && (*temp1 == *temp2); شمېر--) {
		temp1++;
		temp2++;
	}

	إذا (شمېر > 0)
		أرجع *(بینښه حرف *)temp1 - *(بینښه حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t شمېر)
{
	ثابت بینښه حرف *لنډمهاله = s;

	طالما (شمېر > 0) {
		إذا ((بینښه حرف)c == *لنډمهاله++) {
			أرجع (فراغ *)(لنډمهاله - 1);
		}
		شمېر--;
	}

	أرجع عدم;
}
