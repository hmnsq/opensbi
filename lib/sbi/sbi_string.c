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

#অন্তর্ভাব <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
পূর্ণাঙ্ক sbi_strcmp(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b)
{
	/* search first diff or end of string */
	প্রতি (; *a == *b && *a != '\0'; a++, b++)
		;

	প্রত্যাগম *a - *b;
}

পূর্ণাঙ্ক sbi_strncmp(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b, size_t গুনতি)
{
	/* search first diff or end of string */
	প্রতি (; গুনতি > 0 && *a == *b && *a != '\0'; a++, b++, গুনতি--)
		;

	/* No difference till the end */
	যদি (!গুনতি)
		প্রত্যাগম 0;

	প্রত্যাগম *a - *b;
}

size_t sbi_strlen(ধ্রুব বর্ণ *str)
{
	unsigned long ret = 0;

	যাবত্ (*str != '\0') {
		ret++;
		str++;
	}

	প্রত্যাগম ret;
}

size_t sbi_strnlen(ধ্রুব বর্ণ *str, size_t গুনতি)
{
	unsigned long ret = 0;

	যাবত্ (*str != '\0' && ret < গুনতি) {
		ret++;
		str++;
	}

	প্রত্যাগম ret;
}

বর্ণ *sbi_strcpy(বর্ণ *dest, ধ্রুব বর্ণ *src)
{
	বর্ণ *ret = dest;

	যাবত্ ((*dest++ = *src++) != '\0') {
	}
	প্রত্যাগম ret;
}

বর্ণ *sbi_strncpy(বর্ণ *dest, ধ্রুব বর্ণ *src, size_t গুনতি)
{
	বর্ণ *tmp = dest;

	যাবত্ (গুনতি) {
		যদি ((*tmp = *src) != 0)
			src++;
		tmp++;
		গুনতি--;
	}
	প্রত্যাগম dest;
}

বর্ণ *sbi_strchr(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক c)
{
	কুরু {
		যদি (*s == (বর্ণ)c)
			প্রত্যাগম (বর্ণ *)s;
	} যাবত্ (*s++ != '\0');

	প্রত্যাগম রিক্ত;
}

বর্ণ *sbi_strrchr(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক c)
{
	ধ্রুব বর্ণ *last = s + sbi_strlen(s);

	যাবত্ (last > s && *last != (বর্ণ)c)
		last--;

	যদি (*last != (বর্ণ)c)
		প্রত্যাগম রিক্ত;
	অন্যথা
		প্রত্যাগম (বর্ণ *)last;
}
শূন্য *sbi_memset(শূন্য *s, পূর্ণাঙ্ক c, size_t গুনতি)
{
	বর্ণ *temp = s;

	যাবত্ (গুনতি > 0) {
		গুনতি--;
		*temp++ = c;
	}

	প্রত্যাগম s;
}

শূন্য *sbi_memcpy(শূন্য *dest, ধ্রুব শূন্য *src, size_t গুনতি)
{
	বর্ণ *temp1	  = dest;
	ধ্রুব বর্ণ *temp2 = src;

	যাবত্ (গুনতি > 0) {
		*temp1++ = *temp2++;
		গুনতি--;
	}

	প্রত্যাগম dest;
}

শূন্য *sbi_memmove(শূন্য *dest, ধ্রুব শূন্য *src, size_t গুনতি)
{
	বর্ণ *temp1	  = (বর্ণ *)dest;
	ধ্রুব বর্ণ *temp2 = (বর্ণ *)src;

	যদি (src == dest)
		প্রত্যাগম dest;

	যদি (dest < src) {
		যাবত্ (গুনতি > 0) {
			*temp1++ = *temp2++;
			গুনতি--;
		}
	} অন্যথা {
		temp1 = (বর্ণ *)dest + গুনতি - 1;
		temp2 = (বর্ণ *)src + গুনতি - 1;

		যাবত্ (গুনতি > 0) {
			*temp1-- = *temp2--;
			গুনতি--;
		}
	}

	প্রত্যাগম dest;
}

পূর্ণাঙ্ক sbi_memcmp(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2, size_t গুনতি)
{
	ধ্রুব বর্ণ *temp1 = s1;
	ধ্রুব বর্ণ *temp2 = s2;

	প্রতি (; গুনতি > 0 && (*temp1 == *temp2); গুনতি--) {
		temp1++;
		temp2++;
	}

	যদি (গুনতি > 0)
		প্রত্যাগম *(unsigned বর্ণ *)temp1 - *(unsigned বর্ণ *)temp2;
	অন্যথা
		প্রত্যাগম 0;
}

শূন্য *sbi_memchr(ধ্রুব শূন্য *s, পূর্ণাঙ্ক c, size_t গুনতি)
{
	ধ্রুব unsigned বর্ণ *temp = s;

	যাবত্ (গুনতি > 0) {
		যদি ((unsigned বর্ণ)c == *temp++) {
			প্রত্যাগম (শূন্য *)(temp - 1);
		}
		গুনতি--;
	}

	প্রত্যাগম রিক্ত;
}
