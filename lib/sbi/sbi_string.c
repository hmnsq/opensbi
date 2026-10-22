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

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_strcmp(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b)
{
	/* search first diff or end of string */
	ପାଇଁ (; *a == *b && *a != '\0'; a++, b++)
		;

	ଫେରନ୍ତୁ *a - *b;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_strncmp(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, size_t ଗଣନା_2)
{
	/* search first diff or end of string */
	ପାଇଁ (; ଗଣନା_2 > 0 && *a == *b && *a != '\0'; a++, b++, ଗଣନା_2--)
		;

	/* No difference till the end */
	ଯଦି (!ଗଣନା_2)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ *a - *b;
}

size_t sbi_strlen(ଅଚଳ ଅକ୍ଷର *str)
{
	unsigned long ret = 0;

	ଯେତେବେଳେ (*str != '\0') {
		ret++;
		str++;
	}

	ଫେରନ୍ତୁ ret;
}

size_t sbi_strnlen(ଅଚଳ ଅକ୍ଷର *str, size_t ଗଣନା_2)
{
	unsigned long ret = 0;

	ଯେତେବେଳେ (*str != '\0' && ret < ଗଣନା_2) {
		ret++;
		str++;
	}

	ଫେରନ୍ତୁ ret;
}

ଅକ୍ଷର *sbi_strcpy(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src)
{
	ଅକ୍ଷର *ret = dest;

	ଯେତେବେଳେ ((*dest++ = *src++) != '\0') {
	}
	ଫେରନ୍ତୁ ret;
}

ଅକ୍ଷର *sbi_strncpy(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src, size_t ଗଣନା_2)
{
	ଅକ୍ଷର *tmp = dest;

	ଯେତେବେଳେ (ଗଣନା_2) {
		ଯଦି ((*tmp = *src) != 0)
			src++;
		tmp++;
		ଗଣନା_2--;
	}
	ଫେରନ୍ତୁ dest;
}

ଅକ୍ଷର *sbi_strchr(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
	କର {
		ଯଦି (*s == (ଅକ୍ଷର)c)
			ଫେରନ୍ତୁ (ଅକ୍ଷର *)s;
	} ଯେତେବେଳେ (*s++ != '\0');

	ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର *sbi_strrchr(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
	ଅଚଳ ଅକ୍ଷର *last = s + sbi_strlen(s);

	ଯେତେବେଳେ (last > s && *last != (ଅକ୍ଷର)c)
		last--;

	ଯଦି (*last != (ଅକ୍ଷର)c)
		ଫେରନ୍ତୁ ଖାଲି;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ (ଅକ୍ଷର *)last;
}
ଶୂନ୍ୟ *sbi_memset(ଶୂନ୍ୟ *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, size_t ଗଣନା_2)
{
	ଅକ୍ଷର *temp = s;

	ଯେତେବେଳେ (ଗଣନା_2 > 0) {
		ଗଣନା_2--;
		*temp++ = c;
	}

	ଫେରନ୍ତୁ s;
}

ଶୂନ୍ୟ *sbi_memcpy(ଶୂନ୍ୟ *dest, ଅଚଳ ଶୂନ୍ୟ *src, size_t ଗଣନା_2)
{
	ଅକ୍ଷର *temp1	  = dest;
	ଅଚଳ ଅକ୍ଷର *temp2 = src;

	ଯେତେବେଳେ (ଗଣନା_2 > 0) {
		*temp1++ = *temp2++;
		ଗଣନା_2--;
	}

	ଫେରନ୍ତୁ dest;
}

ଶୂନ୍ୟ *sbi_memmove(ଶୂନ୍ୟ *dest, ଅଚଳ ଶୂନ୍ୟ *src, size_t ଗଣନା_2)
{
	ଅକ୍ଷର *temp1	  = (ଅକ୍ଷର *)dest;
	ଅଚଳ ଅକ୍ଷର *temp2 = (ଅକ୍ଷର *)src;

	ଯଦି (src == dest)
		ଫେରନ୍ତୁ dest;

	ଯଦି (dest < src) {
		ଯେତେବେଳେ (ଗଣନା_2 > 0) {
			*temp1++ = *temp2++;
			ଗଣନା_2--;
		}
	} ଅନ୍ୟଥା {
		temp1 = (ଅକ୍ଷର *)dest + ଗଣନା_2 - 1;
		temp2 = (ଅକ୍ଷର *)src + ଗଣନା_2 - 1;

		ଯେତେବେଳେ (ଗଣନା_2 > 0) {
			*temp1-- = *temp2--;
			ଗଣନା_2--;
		}
	}

	ଫେରନ୍ତୁ dest;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_memcmp(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2, size_t ଗଣନା_2)
{
	ଅଚଳ ଅକ୍ଷର *temp1 = s1;
	ଅଚଳ ଅକ୍ଷର *temp2 = s2;

	ପାଇଁ (; ଗଣନା_2 > 0 && (*temp1 == *temp2); ଗଣନା_2--) {
		temp1++;
		temp2++;
	}

	ଯଦି (ଗଣନା_2 > 0)
		ଫେରନ୍ତୁ *(unsigned ଅକ୍ଷର *)temp1 - *(unsigned ଅକ୍ଷର *)temp2;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ *sbi_memchr(ଅଚଳ ଶୂନ୍ୟ *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, size_t ଗଣନା_2)
{
	ଅଚଳ unsigned ଅକ୍ଷର *temp = s;

	ଯେତେବେଳେ (ଗଣନା_2 > 0) {
		ଯଦି ((unsigned ଅକ୍ଷର)c == *temp++) {
			ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)(temp - 1);
		}
		ଗଣନା_2--;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}
