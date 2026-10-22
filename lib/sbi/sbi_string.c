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

#অন্তর্ভাৱ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
পূর্ণাঙ্ক sbi_strcmp(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b)
{
	/* search first diff or end of string */
	বাবে (; *a == *b && *a != '\0'; a++, b++)
		;

	ঘূৰাই *a - *b;
}

পূর্ণাঙ্ক sbi_strncmp(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b, size_t গুনতি)
{
	/* search first diff or end of string */
	বাবে (; গুনতি > 0 && *a == *b && *a != '\0'; a++, b++, গুনতি--)
		;

	/* No difference till the end */
	যদি (!গুনতি)
		ঘূৰাই 0;

	ঘূৰাই *a - *b;
}

size_t sbi_strlen(ধ্রুৱ ৱর্ণ *str)
{
	unsigned long ret = 0;

	যেতিয়ালৈ (*str != '\0') {
		ret++;
		str++;
	}

	ঘূৰাই ret;
}

size_t sbi_strnlen(ধ্রুৱ ৱর্ণ *str, size_t গুনতি)
{
	unsigned long ret = 0;

	যেতিয়ালৈ (*str != '\0' && ret < গুনতি) {
		ret++;
		str++;
	}

	ঘূৰাই ret;
}

ৱর্ণ *sbi_strcpy(ৱর্ণ *dest, ধ্রুৱ ৱর্ণ *src)
{
	ৱর্ণ *ret = dest;

	যেতিয়ালৈ ((*dest++ = *src++) != '\0') {
	}
	ঘূৰাই ret;
}

ৱর্ণ *sbi_strncpy(ৱর্ণ *dest, ধ্রুৱ ৱর্ণ *src, size_t গুনতি)
{
	ৱর্ণ *tmp = dest;

	যেতিয়ালৈ (গুনতি) {
		যদি ((*tmp = *src) != 0)
			src++;
		tmp++;
		গুনতি--;
	}
	ঘূৰাই dest;
}

ৱর্ণ *sbi_strchr(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক c)
{
	কৰা {
		যদি (*s == (ৱর্ণ)c)
			ঘূৰাই (ৱর্ণ *)s;
	} যেতিয়ালৈ (*s++ != '\0');

	ঘূৰাই রিক্ত;
}

ৱর্ণ *sbi_strrchr(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক c)
{
	ধ্রুৱ ৱর্ণ *last = s + sbi_strlen(s);

	যেতিয়ালৈ (last > s && *last != (ৱর্ণ)c)
		last--;

	যদি (*last != (ৱর্ণ)c)
		ঘূৰাই রিক্ত;
	অন্যথা
		ঘূৰাই (ৱর্ণ *)last;
}
শূন্য *sbi_memset(শূন্য *s, পূর্ণাঙ্ক c, size_t গুনতি)
{
	ৱর্ণ *temp = s;

	যেতিয়ালৈ (গুনতি > 0) {
		গুনতি--;
		*temp++ = c;
	}

	ঘূৰাই s;
}

শূন্য *sbi_memcpy(শূন্য *dest, ধ্রুৱ শূন্য *src, size_t গুনতি)
{
	ৱর্ণ *temp1	  = dest;
	ধ্রুৱ ৱর্ণ *temp2 = src;

	যেতিয়ালৈ (গুনতি > 0) {
		*temp1++ = *temp2++;
		গুনতি--;
	}

	ঘূৰাই dest;
}

শূন্য *sbi_memmove(শূন্য *dest, ধ্রুৱ শূন্য *src, size_t গুনতি)
{
	ৱর্ণ *temp1	  = (ৱর্ণ *)dest;
	ধ্রুৱ ৱর্ণ *temp2 = (ৱর্ণ *)src;

	যদি (src == dest)
		ঘূৰাই dest;

	যদি (dest < src) {
		যেতিয়ালৈ (গুনতি > 0) {
			*temp1++ = *temp2++;
			গুনতি--;
		}
	} অন্যথা {
		temp1 = (ৱর্ণ *)dest + গুনতি - 1;
		temp2 = (ৱর্ণ *)src + গুনতি - 1;

		যেতিয়ালৈ (গুনতি > 0) {
			*temp1-- = *temp2--;
			গুনতি--;
		}
	}

	ঘূৰাই dest;
}

পূর্ণাঙ্ক sbi_memcmp(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2, size_t গুনতি)
{
	ধ্রুৱ ৱর্ণ *temp1 = s1;
	ধ্রুৱ ৱর্ণ *temp2 = s2;

	বাবে (; গুনতি > 0 && (*temp1 == *temp2); গুনতি--) {
		temp1++;
		temp2++;
	}

	যদি (গুনতি > 0)
		ঘূৰাই *(unsigned ৱর্ণ *)temp1 - *(unsigned ৱর্ণ *)temp2;
	অন্যথা
		ঘূৰাই 0;
}

শূন্য *sbi_memchr(ধ্রুৱ শূন্য *s, পূর্ণাঙ্ক c, size_t গুনতি)
{
	ধ্রুৱ unsigned ৱর্ণ *temp = s;

	যেতিয়ালৈ (গুনতি > 0) {
		যদি ((unsigned ৱর্ণ)c == *temp++) {
			ঘূৰাই (শূন্য *)(temp - 1);
		}
		গুনতি--;
	}

	ঘূৰাই রিক্ত;
}
