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

#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
ಪೂರ್ಣಾಙ್ಕ sbi_strcmp(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b)
{
	/* search first diff or end of string */
	ಪ್ರತಿ (; *a == *b && *a != '\0'; a++, b++)
		;

	ಪ್ರತ್ಯಾಗಮ *a - *b;
}

ಪೂರ್ಣಾಙ್ಕ sbi_strncmp(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, size_t ಎಣಿಕೆ)
{
	/* search first diff or end of string */
	ಪ್ರತಿ (; ಎಣಿಕೆ > 0 && *a == *b && *a != '\0'; a++, b++, ಎಣಿಕೆ--)
		;

	/* No difference till the end */
	ಯದಿ (!ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ *a - *b;
}

size_t sbi_strlen(ಧ್ರುವ ವರ್ಣ *str)
{
	unsigned long ret = 0;

	ಯಾವತ್ (*str != '\0') {
		ret++;
		str++;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

size_t sbi_strnlen(ಧ್ರುವ ವರ್ಣ *str, size_t ಎಣಿಕೆ)
{
	unsigned long ret = 0;

	ಯಾವತ್ (*str != '\0' && ret < ಎಣಿಕೆ) {
		ret++;
		str++;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ವರ್ಣ *sbi_strcpy(ವರ್ಣ *dest, ಧ್ರುವ ವರ್ಣ *src)
{
	ವರ್ಣ *ret = dest;

	ಯಾವತ್ ((*dest++ = *src++) != '\0') {
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

ವರ್ಣ *sbi_strncpy(ವರ್ಣ *dest, ಧ್ರುವ ವರ್ಣ *src, size_t ಎಣಿಕೆ)
{
	ವರ್ಣ *tmp = dest;

	ಯಾವತ್ (ಎಣಿಕೆ) {
		ಯದಿ ((*tmp = *src) != 0)
			src++;
		tmp++;
		ಎಣಿಕೆ--;
	}
	ಪ್ರತ್ಯಾಗಮ dest;
}

ವರ್ಣ *sbi_strchr(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ c)
{
	ಕುರು {
		ಯದಿ (*s == (ವರ್ಣ)c)
			ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)s;
	} ಯಾವತ್ (*s++ != '\0');

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ *sbi_strrchr(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ c)
{
	ಧ್ರುವ ವರ್ಣ *last = s + sbi_strlen(s);

	ಯಾವತ್ (last > s && *last != (ವರ್ಣ)c)
		last--;

	ಯದಿ (*last != (ವರ್ಣ)c)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)last;
}
ಶೂನ್ಯ *sbi_memset(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ c, size_t ಎಣಿಕೆ)
{
	ವರ್ಣ *temp = s;

	ಯಾವತ್ (ಎಣಿಕೆ > 0) {
		ಎಣಿಕೆ--;
		*temp++ = c;
	}

	ಪ್ರತ್ಯಾಗಮ s;
}

ಶೂನ್ಯ *sbi_memcpy(ಶೂನ್ಯ *dest, ಧ್ರುವ ಶೂನ್ಯ *src, size_t ಎಣಿಕೆ)
{
	ವರ್ಣ *temp1	  = dest;
	ಧ್ರುವ ವರ್ಣ *temp2 = src;

	ಯಾವತ್ (ಎಣಿಕೆ > 0) {
		*temp1++ = *temp2++;
		ಎಣಿಕೆ--;
	}

	ಪ್ರತ್ಯಾಗಮ dest;
}

ಶೂನ್ಯ *sbi_memmove(ಶೂನ್ಯ *dest, ಧ್ರುವ ಶೂನ್ಯ *src, size_t ಎಣಿಕೆ)
{
	ವರ್ಣ *temp1	  = (ವರ್ಣ *)dest;
	ಧ್ರುವ ವರ್ಣ *temp2 = (ವರ್ಣ *)src;

	ಯದಿ (src == dest)
		ಪ್ರತ್ಯಾಗಮ dest;

	ಯದಿ (dest < src) {
		ಯಾವತ್ (ಎಣಿಕೆ > 0) {
			*temp1++ = *temp2++;
			ಎಣಿಕೆ--;
		}
	} ಅನ್ಯಥಾ {
		temp1 = (ವರ್ಣ *)dest + ಎಣಿಕೆ - 1;
		temp2 = (ವರ್ಣ *)src + ಎಣಿಕೆ - 1;

		ಯಾವತ್ (ಎಣಿಕೆ > 0) {
			*temp1-- = *temp2--;
			ಎಣಿಕೆ--;
		}
	}

	ಪ್ರತ್ಯಾಗಮ dest;
}

ಪೂರ್ಣಾಙ್ಕ sbi_memcmp(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2, size_t ಎಣಿಕೆ)
{
	ಧ್ರುವ ವರ್ಣ *temp1 = s1;
	ಧ್ರುವ ವರ್ಣ *temp2 = s2;

	ಪ್ರತಿ (; ಎಣಿಕೆ > 0 && (*temp1 == *temp2); ಎಣಿಕೆ--) {
		temp1++;
		temp2++;
	}

	ಯದಿ (ಎಣಿಕೆ > 0)
		ಪ್ರತ್ಯಾಗಮ *(unsigned ವರ್ಣ *)temp1 - *(unsigned ವರ್ಣ *)temp2;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ *sbi_memchr(ಧ್ರುವ ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ c, size_t ಎಣಿಕೆ)
{
	ಧ್ರುವ unsigned ವರ್ಣ *temp = s;

	ಯಾವತ್ (ಎಣಿಕೆ > 0) {
		ಯದಿ ((unsigned ವರ್ಣ)c == *temp++) {
			ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)(temp - 1);
		}
		ಎಣಿಕೆ--;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
