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

#شامل <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
صحیح sbi_strcmp(ثابت نویسه *a, ثابت نویسه *b)
{
	/* search first diff or end of string */
	برای (; *a == *b && *a != '\0'; a++, b++)
		;

	بازگشت *a - *b;
}

صحیح sbi_strncmp(ثابت نویسه *a, ثابت نویسه *b, size_t شمار)
{
	/* search first diff or end of string */
	برای (; شمار > 0 && *a == *b && *a != '\0'; a++, b++, شمار--)
		;

	/* No difference till the end */
	اگر (!شمار)
		بازگشت 0;

	بازگشت *a - *b;
}

size_t sbi_strlen(ثابت نویسه *str)
{
	unsigned long ret = 0;

	وقتی (*str != '\0') {
		ret++;
		str++;
	}

	بازگشت ret;
}

size_t sbi_strnlen(ثابت نویسه *str, size_t شمار)
{
	unsigned long ret = 0;

	وقتی (*str != '\0' && ret < شمار) {
		ret++;
		str++;
	}

	بازگشت ret;
}

نویسه *sbi_strcpy(نویسه *dest, ثابت نویسه *src)
{
	نویسه *ret = dest;

	وقتی ((*dest++ = *src++) != '\0') {
	}
	بازگشت ret;
}

نویسه *sbi_strncpy(نویسه *dest, ثابت نویسه *src, size_t شمار)
{
	نویسه *tmp = dest;

	وقتی (شمار) {
		اگر ((*tmp = *src) != 0)
			src++;
		tmp++;
		شمار--;
	}
	بازگشت dest;
}

نویسه *sbi_strchr(ثابت نویسه *s, صحیح c)
{
	انجامبده {
		اگر (*s == (نویسه)c)
			بازگشت (نویسه *)s;
	} وقتی (*s++ != '\0');

	بازگشت پوچ;
}

نویسه *sbi_strrchr(ثابت نویسه *s, صحیح c)
{
	ثابت نویسه *last = s + sbi_strlen(s);

	وقتی (last > s && *last != (نویسه)c)
		last--;

	اگر (*last != (نویسه)c)
		بازگشت پوچ;
	وگرنه
		بازگشت (نویسه *)last;
}
تهی *sbi_memset(تهی *s, صحیح c, size_t شمار)
{
	نویسه *temp = s;

	وقتی (شمار > 0) {
		شمار--;
		*temp++ = c;
	}

	بازگشت s;
}

تهی *sbi_memcpy(تهی *dest, ثابت تهی *src, size_t شمار)
{
	نویسه *temp1	  = dest;
	ثابت نویسه *temp2 = src;

	وقتی (شمار > 0) {
		*temp1++ = *temp2++;
		شمار--;
	}

	بازگشت dest;
}

تهی *sbi_memmove(تهی *dest, ثابت تهی *src, size_t شمار)
{
	نویسه *temp1	  = (نویسه *)dest;
	ثابت نویسه *temp2 = (نویسه *)src;

	اگر (src == dest)
		بازگشت dest;

	اگر (dest < src) {
		وقتی (شمار > 0) {
			*temp1++ = *temp2++;
			شمار--;
		}
	} وگرنه {
		temp1 = (نویسه *)dest + شمار - 1;
		temp2 = (نویسه *)src + شمار - 1;

		وقتی (شمار > 0) {
			*temp1-- = *temp2--;
			شمار--;
		}
	}

	بازگشت dest;
}

صحیح sbi_memcmp(ثابت تهی *s1, ثابت تهی *s2, size_t شمار)
{
	ثابت نویسه *temp1 = s1;
	ثابت نویسه *temp2 = s2;

	برای (; شمار > 0 && (*temp1 == *temp2); شمار--) {
		temp1++;
		temp2++;
	}

	اگر (شمار > 0)
		بازگشت *(unsigned نویسه *)temp1 - *(unsigned نویسه *)temp2;
	وگرنه
		بازگشت 0;
}

تهی *sbi_memchr(ثابت تهی *s, صحیح c, size_t شمار)
{
	ثابت unsigned نویسه *temp = s;

	وقتی (شمار > 0) {
		اگر ((unsigned نویسه)c == *temp++) {
			بازگشت (تهی *)(temp - 1);
		}
		شمار--;
	}

	بازگشت پوچ;
}
