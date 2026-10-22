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
পূর্ণসংখ্যা sbi_strcmp(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b)
{
	/* search first diff or end of string */
	জন্য (; *a == *b && *a != '\0'; a++, b++)
		;

	ফেরত *a - *b;
}

পূর্ণসংখ্যা sbi_strncmp(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, size_t গুনতি)
{
	/* search first diff or end of string */
	জন্য (; গুনতি > 0 && *a == *b && *a != '\0'; a++, b++, গুনতি--)
		;

	/* No difference till the end */
	যদি (!গুনতি)
		ফেরত 0;

	ফেরত *a - *b;
}

size_t sbi_strlen(ধ্রুবক অক্ষর *str)
{
	unsigned long ret = 0;

	যখন (*str != '\0') {
		ret++;
		str++;
	}

	ফেরত ret;
}

size_t sbi_strnlen(ধ্রুবক অক্ষর *str, size_t গুনতি)
{
	unsigned long ret = 0;

	যখন (*str != '\0' && ret < গুনতি) {
		ret++;
		str++;
	}

	ফেরত ret;
}

অক্ষর *sbi_strcpy(অক্ষর *dest, ধ্রুবক অক্ষর *src)
{
	অক্ষর *ret = dest;

	যখন ((*dest++ = *src++) != '\0') {
	}
	ফেরত ret;
}

অক্ষর *sbi_strncpy(অক্ষর *dest, ধ্রুবক অক্ষর *src, size_t গুনতি)
{
	অক্ষর *tmp = dest;

	যখন (গুনতি) {
		যদি ((*tmp = *src) != 0)
			src++;
		tmp++;
		গুনতি--;
	}
	ফেরত dest;
}

অক্ষর *sbi_strchr(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা c)
{
	করো {
		যদি (*s == (অক্ষর)c)
			ফেরত (অক্ষর *)s;
	} যখন (*s++ != '\0');

	ফেরত ফাঁকা;
}

অক্ষর *sbi_strrchr(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা c)
{
	ধ্রুবক অক্ষর *last = s + sbi_strlen(s);

	যখন (last > s && *last != (অক্ষর)c)
		last--;

	যদি (*last != (অক্ষর)c)
		ফেরত ফাঁকা;
	অন্যথা
		ফেরত (অক্ষর *)last;
}
শূন্য *sbi_memset(শূন্য *s, পূর্ণসংখ্যা c, size_t গুনতি)
{
	অক্ষর *অস্থায়ী = s;

	যখন (গুনতি > 0) {
		গুনতি--;
		*অস্থায়ী++ = c;
	}

	ফেরত s;
}

শূন্য *sbi_memcpy(শূন্য *dest, ধ্রুবক শূন্য *src, size_t গুনতি)
{
	অক্ষর *temp1	  = dest;
	ধ্রুবক অক্ষর *temp2 = src;

	যখন (গুনতি > 0) {
		*temp1++ = *temp2++;
		গুনতি--;
	}

	ফেরত dest;
}

শূন্য *sbi_memmove(শূন্য *dest, ধ্রুবক শূন্য *src, size_t গুনতি)
{
	অক্ষর *temp1	  = (অক্ষর *)dest;
	ধ্রুবক অক্ষর *temp2 = (অক্ষর *)src;

	যদি (src == dest)
		ফেরত dest;

	যদি (dest < src) {
		যখন (গুনতি > 0) {
			*temp1++ = *temp2++;
			গুনতি--;
		}
	} অন্যথা {
		temp1 = (অক্ষর *)dest + গুনতি - 1;
		temp2 = (অক্ষর *)src + গুনতি - 1;

		যখন (গুনতি > 0) {
			*temp1-- = *temp2--;
			গুনতি--;
		}
	}

	ফেরত dest;
}

পূর্ণসংখ্যা sbi_memcmp(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2, size_t গুনতি)
{
	ধ্রুবক অক্ষর *temp1 = s1;
	ধ্রুবক অক্ষর *temp2 = s2;

	জন্য (; গুনতি > 0 && (*temp1 == *temp2); গুনতি--) {
		temp1++;
		temp2++;
	}

	যদি (গুনতি > 0)
		ফেরত *(unsigned অক্ষর *)temp1 - *(unsigned অক্ষর *)temp2;
	অন্যথা
		ফেরত 0;
}

শূন্য *sbi_memchr(ধ্রুবক শূন্য *s, পূর্ণসংখ্যা c, size_t গুনতি)
{
	ধ্রুবক unsigned অক্ষর *অস্থায়ী = s;

	যখন (গুনতি > 0) {
		যদি ((unsigned অক্ষর)c == *অস্থায়ী++) {
			ফেরত (শূন্য *)(অস্থায়ী - 1);
		}
		গুনতি--;
	}

	ফেরত ফাঁকা;
}
