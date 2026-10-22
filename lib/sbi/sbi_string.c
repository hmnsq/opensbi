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

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t ڳڻپ)
{
	/* search first diff or end of string */
	لكل (; ڳڻپ > 0 && *a == *b && *a != '\0'; a++, b++, ڳڻپ--)
		;

	/* No difference till the end */
	إذا (!ڳڻپ)
		أرجع 0;

	أرجع *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	بنانشان ڊگهو ret = 0;

	طالما (*str != '\0') {
		ret++;
		str++;
	}

	أرجع ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t ڳڻپ)
{
	بنانشان ڊگهو ret = 0;

	طالما (*str != '\0' && ret < ڳڻپ) {
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

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t ڳڻپ)
{
	حرف *tmp = dest;

	طالما (ڳڻپ) {
		إذا ((*tmp = *src) != 0)
			src++;
		tmp++;
		ڳڻپ--;
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
	ثابت حرف *آخري = s + sbi_strlen(s);

	طالما (آخري > s && *آخري != (حرف)c)
		آخري--;

	إذا (*آخري != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)آخري;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t ڳڻپ)
{
	حرف *عارضي = s;

	طالما (ڳڻپ > 0) {
		ڳڻپ--;
		*عارضي++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t ڳڻپ)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (ڳڻپ > 0) {
		*temp1++ = *temp2++;
		ڳڻپ--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t ڳڻپ)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	إذا (src == dest)
		أرجع dest;

	إذا (dest < src) {
		طالما (ڳڻپ > 0) {
			*temp1++ = *temp2++;
			ڳڻپ--;
		}
	} وإلا {
		temp1 = (حرف *)dest + ڳڻپ - 1;
		temp2 = (حرف *)src + ڳڻپ - 1;

		طالما (ڳڻپ > 0) {
			*temp1-- = *temp2--;
			ڳڻپ--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t ڳڻپ)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; ڳڻپ > 0 && (*temp1 == *temp2); ڳڻپ--) {
		temp1++;
		temp2++;
	}

	إذا (ڳڻپ > 0)
		أرجع *(بنانشان حرف *)temp1 - *(بنانشان حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t ڳڻپ)
{
	ثابت بنانشان حرف *عارضي = s;

	طالما (ڳڻپ > 0) {
		إذا ((بنانشان حرف)c == *عارضي++) {
			أرجع (فراغ *)(عارضي - 1);
		}
		ڳڻپ--;
	}

	أرجع عدم;
}
