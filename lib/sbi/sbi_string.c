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

#અન્તર્ભાવ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
પૂર્ણાંક sbi_strcmp(અચલ અક્ષર *a, અચલ અક્ષર *b)
{
	/* search first diff or end of string */
	માટે (; *a == *b && *a != '\0'; a++, b++)
		;

	પાછા *a - *b;
}

પૂર્ણાંક sbi_strncmp(અચલ અક્ષર *a, અચલ અક્ષર *b, size_t ગણતરી)
{
	/* search first diff or end of string */
	માટે (; ગણતરી > 0 && *a == *b && *a != '\0'; a++, b++, ગણતરી--)
		;

	/* No difference till the end */
	જો (!ગણતરી)
		પાછા 0;

	પાછા *a - *b;
}

size_t sbi_strlen(અચલ અક્ષર *str)
{
	unsigned long ret = 0;

	જ્યારે (*str != '\0') {
		ret++;
		str++;
	}

	પાછા ret;
}

size_t sbi_strnlen(અચલ અક્ષર *str, size_t ગણતરી)
{
	unsigned long ret = 0;

	જ્યારે (*str != '\0' && ret < ગણતરી) {
		ret++;
		str++;
	}

	પાછા ret;
}

અક્ષર *sbi_strcpy(અક્ષર *dest, અચલ અક્ષર *src)
{
	અક્ષર *ret = dest;

	જ્યારે ((*dest++ = *src++) != '\0') {
	}
	પાછા ret;
}

અક્ષર *sbi_strncpy(અક્ષર *dest, અચલ અક્ષર *src, size_t ગણતરી)
{
	અક્ષર *tmp = dest;

	જ્યારે (ગણતરી) {
		જો ((*tmp = *src) != 0)
			src++;
		tmp++;
		ગણતરી--;
	}
	પાછા dest;
}

અક્ષર *sbi_strchr(અચલ અક્ષર *s, પૂર્ણાંક c)
{
	કરો {
		જો (*s == (અક્ષર)c)
			પાછા (અક્ષર *)s;
	} જ્યારે (*s++ != '\0');

	પાછા ખાલી;
}

અક્ષર *sbi_strrchr(અચલ અક્ષર *s, પૂર્ણાંક c)
{
	અચલ અક્ષર *last = s + sbi_strlen(s);

	જ્યારે (last > s && *last != (અક્ષર)c)
		last--;

	જો (*last != (અક્ષર)c)
		પાછા ખાલી;
	નહીંતર
		પાછા (અક્ષર *)last;
}
શૂન્ય *sbi_memset(શૂન્ય *s, પૂર્ણાંક c, size_t ગણતરી)
{
	અક્ષર *temp = s;

	જ્યારે (ગણતરી > 0) {
		ગણતરી--;
		*temp++ = c;
	}

	પાછા s;
}

શૂન્ય *sbi_memcpy(શૂન્ય *dest, અચલ શૂન્ય *src, size_t ગણતરી)
{
	અક્ષર *temp1	  = dest;
	અચલ અક્ષર *temp2 = src;

	જ્યારે (ગણતરી > 0) {
		*temp1++ = *temp2++;
		ગણતરી--;
	}

	પાછા dest;
}

શૂન્ય *sbi_memmove(શૂન્ય *dest, અચલ શૂન્ય *src, size_t ગણતરી)
{
	અક્ષર *temp1	  = (અક્ષર *)dest;
	અચલ અક્ષર *temp2 = (અક્ષર *)src;

	જો (src == dest)
		પાછા dest;

	જો (dest < src) {
		જ્યારે (ગણતરી > 0) {
			*temp1++ = *temp2++;
			ગણતરી--;
		}
	} નહીંતર {
		temp1 = (અક્ષર *)dest + ગણતરી - 1;
		temp2 = (અક્ષર *)src + ગણતરી - 1;

		જ્યારે (ગણતરી > 0) {
			*temp1-- = *temp2--;
			ગણતરી--;
		}
	}

	પાછા dest;
}

પૂર્ણાંક sbi_memcmp(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2, size_t ગણતરી)
{
	અચલ અક્ષર *temp1 = s1;
	અચલ અક્ષર *temp2 = s2;

	માટે (; ગણતરી > 0 && (*temp1 == *temp2); ગણતરી--) {
		temp1++;
		temp2++;
	}

	જો (ગણતરી > 0)
		પાછા *(unsigned અક્ષર *)temp1 - *(unsigned અક્ષર *)temp2;
	નહીંતર
		પાછા 0;
}

શૂન્ય *sbi_memchr(અચલ શૂન્ય *s, પૂર્ણાંક c, size_t ગણતરી)
{
	અચલ unsigned અક્ષર *temp = s;

	જ્યારે (ગણતરી > 0) {
		જો ((unsigned અક્ષર)c == *temp++) {
			પાછા (શૂન્ય *)(temp - 1);
		}
		ગણતરી--;
	}

	પાછા ખાલી;
}
