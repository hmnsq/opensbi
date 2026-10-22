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

صحيح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t count)
{
	/* search first diff or end of string */
	لكل (; count > 0 && *a == *b && *a != '\0'; a++, b++, count--)
		;

	/* No difference till the end */
	𐤀𐤌 (!count)
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

size_t sbi_strnlen(ثابت حرف *str, size_t count)
{
	unsigned long ret = 0;

	طالما (*str != '\0' && ret < count) {
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

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t count)
{
	حرف *tmp = dest;

	طالما (count) {
		𐤀𐤌 ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	أرجع dest;
}

حرف *sbi_strchr(ثابت حرف *s, صحيح c)
{
	𐤐𐤏𐤋 {
		𐤀𐤌 (*s == (حرف)c)
			أرجع (حرف *)s;
	} طالما (*s++ != '\0');

	أرجع عدم;
}

حرف *sbi_strrchr(ثابت حرف *s, صحيح c)
{
	ثابت حرف *last = s + sbi_strlen(s);

	طالما (last > s && *last != (حرف)c)
		last--;

	𐤀𐤌 (*last != (حرف)c)
		أرجع عدم;
	وإلا
		أرجع (حرف *)last;
}
فراغ *sbi_memset(فراغ *s, صحيح c, size_t count)
{
	حرف *temp = s;

	طالما (count > 0) {
		count--;
		*temp++ = c;
	}

	أرجع s;
}

فراغ *sbi_memcpy(فراغ *dest, ثابت فراغ *src, size_t count)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	طالما (count > 0) {
		*temp1++ = *temp2++;
		count--;
	}

	أرجع dest;
}

فراغ *sbi_memmove(فراغ *dest, ثابت فراغ *src, size_t count)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	𐤀𐤌 (src == dest)
		أرجع dest;

	𐤀𐤌 (dest < src) {
		طالما (count > 0) {
			*temp1++ = *temp2++;
			count--;
		}
	} وإلا {
		temp1 = (حرف *)dest + count - 1;
		temp2 = (حرف *)src + count - 1;

		طالما (count > 0) {
			*temp1-- = *temp2--;
			count--;
		}
	}

	أرجع dest;
}

صحيح sbi_memcmp(ثابت فراغ *s1, ثابت فراغ *s2, size_t count)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	لكل (; count > 0 && (*temp1 == *temp2); count--) {
		temp1++;
		temp2++;
	}

	𐤀𐤌 (count > 0)
		أرجع *(unsigned حرف *)temp1 - *(unsigned حرف *)temp2;
	وإلا
		أرجع 0;
}

فراغ *sbi_memchr(ثابت فراغ *s, صحيح c, size_t count)
{
	ثابت unsigned حرف *temp = s;

	طالما (count > 0) {
		𐤀𐤌 ((unsigned حرف)c == *temp++) {
			أرجع (فراغ *)(temp - 1);
		}
		count--;
	}

	أرجع عدم;
}
