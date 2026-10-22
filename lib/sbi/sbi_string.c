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

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t گنتی)
{
	/* search first diff or end of string */
	لكل (; گنتی > 0 && *a == *b && *a != '\0'; a++, b++, گنتی--)
		;

	/* No difference till the end */
	إذا (!گنتی)
		أرجع 0;

	أرجع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	بنانشانی لما ret = 0;

	طالما (*str != '\0') {
		ret++;
		str++;
	}

	أرجع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t گنتی)
{
	بنانشانی لما ret = 0;

	طالما (*str != '\0' && ret < گنتی) {
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

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t گنتی)
{
	حرف *tmp = dest;

	طالما (گنتی) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		گنتی--;
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
	ثابت حرف *آخری = s + sbi_strlen(s);

	طالما (آخری > s && *آخری != (حرف)c)
		آخری--;

	إذا (*آخری != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)آخری;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t گنتی)
{
	حرف *عارضی = s;

	طالما (گنتی > 0) {
		گنتی--;
		*عارضی++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t گنتی)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (گنتی > 0) {
		*temp1++ = *temp2++;
		گنتی--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t گنتی)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		أرجع dest;

	إذا (dest < src) {
		طالما (گنتی > 0) {
			*temp1++ = *temp2++;
			گنتی--;
		}
	} وإلا {
		temp1 = (حرف *)dest + گنتی - 1;
		temp2 = (حرف *)src + گنتی - 1;

		طالما (گنتی > 0) {
			*temp1-- = *temp2--;
			گنتی--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t گنتی)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; گنتی > 0 && (*temp1 == *temp2); گنتی--) {
		temp1++;
		temp2++;
	}

	إذا (گنتی > 0)
		أرجع *(بنانشانی حرف *)temp1 - *(بنانشانی حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t گنتی)
{
	ثابت بنانشانی حرف *عارضی = s;

	طالما (گنتی > 0) {
		إذا ((بنانشانی حرف)c == *عارضی++) {
			أرجع (فراغ *)(عارضی - 1);
		}
		گنتی--;
	}

	أرجع عدم;
}
