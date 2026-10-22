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
صحیح sbi_strcmp(ثابت حرف *a, ثابت حرف *b)
{
	/* search first diff or end of string */
	برائے (; *a == *b && *a != '\0'; a++, b++)
		;

	واپس *a - *b;
}

صحیح sbi_strncmp(ثابت حرف *a, ثابت حرف *b, size_t گنتی)
{
	/* search first diff or end of string */
	برائے (; گنتی > 0 && *a == *b && *a != '\0'; a++, b++, گنتی--)
		;

	/* No difference till the end */
	اگر (!گنتی)
		واپس 0;

	واپس *a - *b;
}

size_t sbi_strlen(ثابت حرف *str)
{
	غیرعلامتی طویل ret = 0;

	جب (*str != '\0') {
		ret++;
		str++;
	}

	واپس ret;
}

size_t sbi_strnlen(ثابت حرف *str, size_t گنتی)
{
	غیرعلامتی طویل ret = 0;

	جب (*str != '\0' && ret < گنتی) {
		ret++;
		str++;
	}

	واپس ret;
}

حرف *sbi_strcpy(حرف *dest, ثابت حرف *src)
{
	حرف *ret = dest;

	جب ((*dest++ = *src++) != '\0') {
	}
	واپس ret;
}

حرف *sbi_strncpy(حرف *dest, ثابت حرف *src, size_t گنتی)
{
	حرف *tmp = dest;

	جب (گنتی) {
		اگر ((*tmp = *src) != 0)
			src++;
		tmp++;
		گنتی--;
	}
	واپس dest;
}

حرف *sbi_strchr(ثابت حرف *s, صحیح c)
{
	کرو {
		اگر (*s == (حرف)c)
			واپس (حرف *)s;
	} جب (*s++ != '\0');

	واپس تہی;
}

حرف *sbi_strrchr(ثابت حرف *s, صحیح c)
{
	ثابت حرف *آخری = s + sbi_strlen(s);

	جب (آخری > s && *آخری != (حرف)c)
		آخری--;

	اگر (*آخری != (حرف)c)
		واپس تہی;
	ورنہ
		واپس (حرف *)آخری;
}
خالی *sbi_memset(خالی *s, صحیح c, size_t گنتی)
{
	حرف *عارضی = s;

	جب (گنتی > 0) {
		گنتی--;
		*عارضی++ = c;
	}

	واپس s;
}

خالی *sbi_memcpy(خالی *dest, ثابت خالی *src, size_t گنتی)
{
	حرف *temp1	  = dest;
	ثابت حرف *temp2 = src;

	جب (گنتی > 0) {
		*temp1++ = *temp2++;
		گنتی--;
	}

	واپس dest;
}

خالی *sbi_memmove(خالی *dest, ثابت خالی *src, size_t گنتی)
{
	حرف *temp1	  = (حرف *)dest;
	ثابت حرف *temp2 = (حرف *)src;

	اگر (src == dest)
		واپس dest;

	اگر (dest < src) {
		جب (گنتی > 0) {
			*temp1++ = *temp2++;
			گنتی--;
		}
	} ورنہ {
		temp1 = (حرف *)dest + گنتی - 1;
		temp2 = (حرف *)src + گنتی - 1;

		جب (گنتی > 0) {
			*temp1-- = *temp2--;
			گنتی--;
		}
	}

	واپس dest;
}

صحیح sbi_memcmp(ثابت خالی *s1, ثابت خالی *s2, size_t گنتی)
{
	ثابت حرف *temp1 = s1;
	ثابت حرف *temp2 = s2;

	برائے (; گنتی > 0 && (*temp1 == *temp2); گنتی--) {
		temp1++;
		temp2++;
	}

	اگر (گنتی > 0)
		واپس *(غیرعلامتی حرف *)temp1 - *(غیرعلامتی حرف *)temp2;
	ورنہ
		واپس 0;
}

خالی *sbi_memchr(ثابت خالی *s, صحیح c, size_t گنتی)
{
	ثابت غیرعلامتی حرف *عارضی = s;

	جب (گنتی > 0) {
		اگر ((غیرعلامتی حرف)c == *عارضی++) {
			واپس (خالی *)(عارضی - 1);
		}
		گنتی--;
	}

	واپس تہی;
}
