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

#அந்தர்பாவ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
முழு sbi_strcmp(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b)
{
	/* search first diff or end of string */
	க்கு (; *a == *b && *a != '\0'; a++, b++)
		;

	திரும்பு *a - *b;
}

முழு sbi_strncmp(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, size_t மொத்தம்)
{
	/* search first diff or end of string */
	க்கு (; மொத்தம் > 0 && *a == *b && *a != '\0'; a++, b++, மொத்தம்--)
		;

	/* No difference till the end */
	எனில் (!மொத்தம்)
		திரும்பு 0;

	திரும்பு *a - *b;
}

size_t sbi_strlen(மாறிலி எழுத்து *str)
{
	unsigned long ret = 0;

	போது (*str != '\0') {
		ret++;
		str++;
	}

	திரும்பு ret;
}

size_t sbi_strnlen(மாறிலி எழுத்து *str, size_t மொத்தம்)
{
	unsigned long ret = 0;

	போது (*str != '\0' && ret < மொத்தம்) {
		ret++;
		str++;
	}

	திரும்பு ret;
}

எழுத்து *sbi_strcpy(எழுத்து *dest, மாறிலி எழுத்து *src)
{
	எழுத்து *ret = dest;

	போது ((*dest++ = *src++) != '\0') {
	}
	திரும்பு ret;
}

எழுத்து *sbi_strncpy(எழுத்து *dest, மாறிலி எழுத்து *src, size_t மொத்தம்)
{
	எழுத்து *tmp = dest;

	போது (மொத்தம்) {
		எனில் ((*tmp = *src) != 0)
			src++;
		tmp++;
		மொத்தம்--;
	}
	திரும்பு dest;
}

எழுத்து *sbi_strchr(மாறிலி எழுத்து *s, முழு c)
{
	செய் {
		எனில் (*s == (எழுத்து)c)
			திரும்பு (எழுத்து *)s;
	} போது (*s++ != '\0');

	திரும்பு வெற்று;
}

எழுத்து *sbi_strrchr(மாறிலி எழுத்து *s, முழு c)
{
	மாறிலி எழுத்து *last = s + sbi_strlen(s);

	போது (last > s && *last != (எழுத்து)c)
		last--;

	எனில் (*last != (எழுத்து)c)
		திரும்பு வெற்று;
	இல்லையெனில்
		திரும்பு (எழுத்து *)last;
}
வெறுமை *sbi_memset(வெறுமை *s, முழு c, size_t மொத்தம்)
{
	எழுத்து *temp = s;

	போது (மொத்தம் > 0) {
		மொத்தம்--;
		*temp++ = c;
	}

	திரும்பு s;
}

வெறுமை *sbi_memcpy(வெறுமை *dest, மாறிலி வெறுமை *src, size_t மொத்தம்)
{
	எழுத்து *temp1	  = dest;
	மாறிலி எழுத்து *temp2 = src;

	போது (மொத்தம் > 0) {
		*temp1++ = *temp2++;
		மொத்தம்--;
	}

	திரும்பு dest;
}

வெறுமை *sbi_memmove(வெறுமை *dest, மாறிலி வெறுமை *src, size_t மொத்தம்)
{
	எழுத்து *temp1	  = (எழுத்து *)dest;
	மாறிலி எழுத்து *temp2 = (எழுத்து *)src;

	எனில் (src == dest)
		திரும்பு dest;

	எனில் (dest < src) {
		போது (மொத்தம் > 0) {
			*temp1++ = *temp2++;
			மொத்தம்--;
		}
	} இல்லையெனில் {
		temp1 = (எழுத்து *)dest + மொத்தம் - 1;
		temp2 = (எழுத்து *)src + மொத்தம் - 1;

		போது (மொத்தம் > 0) {
			*temp1-- = *temp2--;
			மொத்தம்--;
		}
	}

	திரும்பு dest;
}

முழு sbi_memcmp(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2, size_t மொத்தம்)
{
	மாறிலி எழுத்து *temp1 = s1;
	மாறிலி எழுத்து *temp2 = s2;

	க்கு (; மொத்தம் > 0 && (*temp1 == *temp2); மொத்தம்--) {
		temp1++;
		temp2++;
	}

	எனில் (மொத்தம் > 0)
		திரும்பு *(unsigned எழுத்து *)temp1 - *(unsigned எழுத்து *)temp2;
	இல்லையெனில்
		திரும்பு 0;
}

வெறுமை *sbi_memchr(மாறிலி வெறுமை *s, முழு c, size_t மொத்தம்)
{
	மாறிலி unsigned எழுத்து *temp = s;

	போது (மொத்தம் > 0) {
		எனில் ((unsigned எழுத்து)c == *temp++) {
			திரும்பு (வெறுமை *)(temp - 1);
		}
		மொத்தம்--;
	}

	திரும்பு வெற்று;
}
