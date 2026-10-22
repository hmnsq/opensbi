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
ಪೂರ್ಣಾಂಕ sbi_strcmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b)
{
	/* search first diff or end of string */
	ಗಾಗಿ (; *a == *b && *a != '\0'; a++, b++)
		;

	ಹಿಂತಿರುಗಿ *a - *b;
}

ಪೂರ್ಣಾಂಕ sbi_strncmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, size_t ಎಣಿಕೆ_2)
{
	/* search first diff or end of string */
	ಗಾಗಿ (; ಎಣಿಕೆ_2 > 0 && *a == *b && *a != '\0'; a++, b++, ಎಣಿಕೆ_2--)
		;

	/* No difference till the end */
	ಆದರೆ (!ಎಣಿಕೆ_2)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ *a - *b;
}

size_t sbi_strlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	ಆಗ (*str != '\0') {
		ret++;
		str++;
	}

	ಹಿಂತಿರುಗಿ ret;
}

size_t sbi_strnlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t ಎಣಿಕೆ_2)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	ಆಗ (*str != '\0' && ret < ಎಣಿಕೆ_2) {
		ret++;
		str++;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಅಕ್ಷರ *sbi_strcpy(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src)
{
	ಅಕ್ಷರ *ret = dest;

	ಆಗ ((*dest++ = *src++) != '\0') {
	}
	ಹಿಂತಿರುಗಿ ret;
}

ಅಕ್ಷರ *sbi_strncpy(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t ಎಣಿಕೆ_2)
{
	ಅಕ್ಷರ *tmp = dest;

	ಆಗ (ಎಣಿಕೆ_2) {
		ಆದರೆ ((*tmp = *src) != 0)
			src++;
		tmp++;
		ಎಣಿಕೆ_2--;
	}
	ಹಿಂತಿರುಗಿ dest;
}

ಅಕ್ಷರ *sbi_strchr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ c)
{
	ಮಾಡು {
		ಆದರೆ (*s == (ಅಕ್ಷರ)c)
			ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)s;
	} ಆಗ (*s++ != '\0');

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ *sbi_strrchr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ c)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆಯ = s + sbi_strlen(s);

	ಆಗ (ಕೊನೆಯ > s && *ಕೊನೆಯ != (ಅಕ್ಷರ)c)
		ಕೊನೆಯ--;

	ಆದರೆ (*ಕೊನೆಯ != (ಅಕ್ಷರ)c)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)ಕೊನೆಯ;
}
ಶೂನ್ಯ *sbi_memset(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಂಕ c, size_t ಎಣಿಕೆ_2)
{
	ಅಕ್ಷರ *ತಾತ್ಕಾಲಿಕ = s;

	ಆಗ (ಎಣಿಕೆ_2 > 0) {
		ಎಣಿಕೆ_2--;
		*ತಾತ್ಕಾಲಿಕ++ = c;
	}

	ಹಿಂತಿರುಗಿ s;
}

ಶೂನ್ಯ *sbi_memcpy(ಶೂನ್ಯ *dest, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, size_t ಎಣಿಕೆ_2)
{
	ಅಕ್ಷರ *temp1	  = dest;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *temp2 = src;

	ಆಗ (ಎಣಿಕೆ_2 > 0) {
		*temp1++ = *temp2++;
		ಎಣಿಕೆ_2--;
	}

	ಹಿಂತಿರುಗಿ dest;
}

ಶೂನ್ಯ *sbi_memmove(ಶೂನ್ಯ *dest, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, size_t ಎಣಿಕೆ_2)
{
	ಅಕ್ಷರ *temp1	  = (ಅಕ್ಷರ *)dest;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *temp2 = (ಅಕ್ಷರ *)src;

	ಆದರೆ (src == dest)
		ಹಿಂತಿರುಗಿ dest;

	ಆದರೆ (dest < src) {
		ಆಗ (ಎಣಿಕೆ_2 > 0) {
			*temp1++ = *temp2++;
			ಎಣಿಕೆ_2--;
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		temp1 = (ಅಕ್ಷರ *)dest + ಎಣಿಕೆ_2 - 1;
		temp2 = (ಅಕ್ಷರ *)src + ಎಣಿಕೆ_2 - 1;

		ಆಗ (ಎಣಿಕೆ_2 > 0) {
			*temp1-- = *temp2--;
			ಎಣಿಕೆ_2--;
		}
	}

	ಹಿಂತಿರುಗಿ dest;
}

ಪೂರ್ಣಾಂಕ sbi_memcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2, size_t ಎಣಿಕೆ_2)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *temp1 = s1;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *temp2 = s2;

	ಗಾಗಿ (; ಎಣಿಕೆ_2 > 0 && (*temp1 == *temp2); ಎಣಿಕೆ_2--) {
		temp1++;
		temp2++;
	}

	ಆದರೆ (ಎಣಿಕೆ_2 > 0)
		ಹಿಂತಿರುಗಿ *(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)temp1 - *(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)temp2;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ *sbi_memchr(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s, ಪೂರ್ಣಾಂಕ c, size_t ಎಣಿಕೆ_2)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ತಾತ್ಕಾಲಿಕ = s;

	ಆಗ (ಎಣಿಕೆ_2 > 0) {
		ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)c == *ತಾತ್ಕಾಲಿಕ++) {
			ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)(ತಾತ್ಕಾಲಿಕ - 1);
		}
		ಎಣಿಕೆ_2--;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
