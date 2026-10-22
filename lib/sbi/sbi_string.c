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

	إرجاع *a - *b;
}

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t تعداد_2)
{
	/* search first diff or end of string */
	لكل (; تعداد_2 > 0 && *a == *b && *a != '\0'; a++, b++, تعداد_2--)
		;

	/* No difference till the end */
	إذا (!تعداد_2)
		إرجاع 0;

	إرجاع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	unsigned long ret = 0;

	بينما (*str != '\0') {
		ret++;
		str++;
	}

	إرجاع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t تعداد_2)
{
	unsigned long ret = 0;

	بينما (*str != '\0' && ret < تعداد_2) {
		ret++;
		str++;
	}

	إرجاع ret;
}

حرف *sbi_strcpy(حرف *dest, ثابت حرف *src)
{
	حرف *ret = dest;

	بينما ((*dest++ = *src++) != '\0') {
	}
	إرجاع ret;
}

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t تعداد_2)
{
	حرف *tmp = dest;

	بينما (تعداد_2) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		تعداد_2--;
	}
	إرجاع dest;
}

حرف *sbi_strchr(ثابت حرف *s, صحيح c)
{
	افعل {
		إذا (*s == (حرف)c)
			إرجاع (حرف *)s;
	} بينما (*s++ != '\0');

	إرجاع عدم;
}

حرف *sbi_strrchr(ثابت حرف *s, صحيح c)
{
	ثابت حرف *last = s + sbi_strlen(s);

	بينما (last > s && *last != (حرف)c)
		last--;

	إذا (*last != (حرف)c)
		إرجاع عدم;
	وإلا
		إرجاع (حرف *)last;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t تعداد_2)
{
	حرف *temp = s;

	بينما (تعداد_2 > 0) {
		تعداد_2--;
		*temp++ = c;
	}

	إرجاع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t تعداد_2)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	بينما (تعداد_2 > 0) {
		*temp1++ = *temp2++;
		تعداد_2--;
	}

	إرجاع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t تعداد_2)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		إرجاع dest;

	إذا (dest < src) {
		بينما (تعداد_2 > 0) {
			*temp1++ = *temp2++;
			تعداد_2--;
		}
	} وإلا {
		temp1 = (حرف *)dest + تعداد_2 - 1;
		temp2 = (حرف *)src + تعداد_2 - 1;

		بينما (تعداد_2 > 0) {
			*temp1-- = *temp2--;
			تعداد_2--;
		}
	}

	إرجاع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t تعداد_2)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; تعداد_2 > 0 && (*temp1 == *temp2); تعداد_2--) {
		temp1++;
		temp2++;
	}

	إذا (تعداد_2 > 0)
		إرجاع *(unsigned حرف *)temp1 - *(unsigned حرف *)temp2;
	وإلا
		إرجاع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t تعداد_2)
{
	ثابت unsigned حرف *temp = s;

	بينما (تعداد_2 > 0) {
		إذا ((unsigned حرف)c == *temp++) {
			إرجاع (فراغ *)(temp - 1);
		}
		تعداد_2--;
	}

	إرجاع عدم;
}
