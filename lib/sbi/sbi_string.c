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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
ਪੂਰਨ sbi_strcmp(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b)
{
	/* search first diff or end of string */
	ਲਈ (; *a == *b && *a != '\0'; a++, b++)
		;

	ਵਾਪਸ *a - *b;
}

ਪੂਰਨ sbi_strncmp(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, size_t ਗਿਣਤੀ_2)
{
	/* search first diff or end of string */
	ਲਈ (; ਗਿਣਤੀ_2 > 0 && *a == *b && *a != '\0'; a++, b++, ਗਿਣਤੀ_2--)
		;

	/* No difference till the end */
	ਜੇ (!ਗਿਣਤੀ_2)
		ਵਾਪਸ 0;

	ਵਾਪਸ *a - *b;
}

size_t sbi_strlen(ਅਚਲ ਅੱਖਰ *str)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	ਜਦੋਂ (*str != '\0') {
		ret++;
		str++;
	}

	ਵਾਪਸ ret;
}

size_t sbi_strnlen(ਅਚਲ ਅੱਖਰ *str, size_t ਗਿਣਤੀ_2)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	ਜਦੋਂ (*str != '\0' && ret < ਗਿਣਤੀ_2) {
		ret++;
		str++;
	}

	ਵਾਪਸ ret;
}

ਅੱਖਰ *sbi_strcpy(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src)
{
	ਅੱਖਰ *ret = dest;

	ਜਦੋਂ ((*dest++ = *src++) != '\0') {
	}
	ਵਾਪਸ ret;
}

ਅੱਖਰ *sbi_strncpy(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, size_t ਗਿਣਤੀ_2)
{
	ਅੱਖਰ *tmp = dest;

	ਜਦੋਂ (ਗਿਣਤੀ_2) {
		ਜੇ ((*tmp = *src) != 0)
			src++;
		tmp++;
		ਗਿਣਤੀ_2--;
	}
	ਵਾਪਸ dest;
}

ਅੱਖਰ *sbi_strchr(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ c)
{
	ਕਰੋ {
		ਜੇ (*s == (ਅੱਖਰ)c)
			ਵਾਪਸ (ਅੱਖਰ *)s;
	} ਜਦੋਂ (*s++ != '\0');

	ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ *sbi_strrchr(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ c)
{
	ਅਚਲ ਅੱਖਰ *ਆਖਰੀ = s + sbi_strlen(s);

	ਜਦੋਂ (ਆਖਰੀ > s && *ਆਖਰੀ != (ਅੱਖਰ)c)
		ਆਖਰੀ--;

	ਜੇ (*ਆਖਰੀ != (ਅੱਖਰ)c)
		ਵਾਪਸ ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ (ਅੱਖਰ *)ਆਖਰੀ;
}
ਖਾਲੀ *sbi_memset(ਖਾਲੀ *s, ਪੂਰਨ c, size_t ਗਿਣਤੀ_2)
{
	ਅੱਖਰ *ਆਰਜ਼ੀ = s;

	ਜਦੋਂ (ਗਿਣਤੀ_2 > 0) {
		ਗਿਣਤੀ_2--;
		*ਆਰਜ਼ੀ++ = c;
	}

	ਵਾਪਸ s;
}

ਖਾਲੀ *sbi_memcpy(ਖਾਲੀ *dest, ਅਚਲ ਖਾਲੀ *src, size_t ਗਿਣਤੀ_2)
{
	ਅੱਖਰ *temp1	  = dest;
	ਅਚਲ ਅੱਖਰ *temp2 = src;

	ਜਦੋਂ (ਗਿਣਤੀ_2 > 0) {
		*temp1++ = *temp2++;
		ਗਿਣਤੀ_2--;
	}

	ਵਾਪਸ dest;
}

ਖਾਲੀ *sbi_memmove(ਖਾਲੀ *dest, ਅਚਲ ਖਾਲੀ *src, size_t ਗਿਣਤੀ_2)
{
	ਅੱਖਰ *temp1	  = (ਅੱਖਰ *)dest;
	ਅਚਲ ਅੱਖਰ *temp2 = (ਅੱਖਰ *)src;

	ਜੇ (src == dest)
		ਵਾਪਸ dest;

	ਜੇ (dest < src) {
		ਜਦੋਂ (ਗਿਣਤੀ_2 > 0) {
			*temp1++ = *temp2++;
			ਗਿਣਤੀ_2--;
		}
	} ਨਹੀਂਤਾਂ {
		temp1 = (ਅੱਖਰ *)dest + ਗਿਣਤੀ_2 - 1;
		temp2 = (ਅੱਖਰ *)src + ਗਿਣਤੀ_2 - 1;

		ਜਦੋਂ (ਗਿਣਤੀ_2 > 0) {
			*temp1-- = *temp2--;
			ਗਿਣਤੀ_2--;
		}
	}

	ਵਾਪਸ dest;
}

ਪੂਰਨ sbi_memcmp(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2, size_t ਗਿਣਤੀ_2)
{
	ਅਚਲ ਅੱਖਰ *temp1 = s1;
	ਅਚਲ ਅੱਖਰ *temp2 = s2;

	ਲਈ (; ਗਿਣਤੀ_2 > 0 && (*temp1 == *temp2); ਗਿਣਤੀ_2--) {
		temp1++;
		temp2++;
	}

	ਜੇ (ਗਿਣਤੀ_2 > 0)
		ਵਾਪਸ *(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)temp1 - *(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)temp2;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ 0;
}

ਖਾਲੀ *sbi_memchr(ਅਚਲ ਖਾਲੀ *s, ਪੂਰਨ c, size_t ਗਿਣਤੀ_2)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਆਰਜ਼ੀ = s;

	ਜਦੋਂ (ਗਿਣਤੀ_2 > 0) {
		ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)c == *ਆਰਜ਼ੀ++) {
			ਵਾਪਸ (ਖਾਲੀ *)(ਆਰਜ਼ੀ - 1);
		}
		ਗਿਣਤੀ_2--;
	}

	ਵਾਪਸ ਨਿਰਾ;
}
