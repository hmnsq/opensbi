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

#అన్తర్భావ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
పూర్ణాంకం sbi_strcmp(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b)
{
	/* search first diff or end of string */
	కొరకు (; *a == *b && *a != '\0'; a++, b++)
		;

	తిరిగి *a - *b;
}

పూర్ణాంకం sbi_strncmp(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, size_t లెక్క)
{
	/* search first diff or end of string */
	కొరకు (; లెక్క > 0 && *a == *b && *a != '\0'; a++, b++, లెక్క--)
		;

	/* No difference till the end */
	ఒకవేళ (!లెక్క)
		తిరిగి 0;

	తిరిగి *a - *b;
}

size_t sbi_strlen(స్థిరరాశి అక్షరం *str)
{
	గుర్తులేని దీర్ఘ ret = 0;

	అయితే (*str != '\0') {
		ret++;
		str++;
	}

	తిరిగి ret;
}

size_t sbi_strnlen(స్థిరరాశి అక్షరం *str, size_t లెక్క)
{
	గుర్తులేని దీర్ఘ ret = 0;

	అయితే (*str != '\0' && ret < లెక్క) {
		ret++;
		str++;
	}

	తిరిగి ret;
}

అక్షరం *sbi_strcpy(అక్షరం *dest, స్థిరరాశి అక్షరం *src)
{
	అక్షరం *ret = dest;

	అయితే ((*dest++ = *src++) != '\0') {
	}
	తిరిగి ret;
}

అక్షరం *sbi_strncpy(అక్షరం *dest, స్థిరరాశి అక్షరం *src, size_t లెక్క)
{
	అక్షరం *tmp = dest;

	అయితే (లెక్క) {
		ఒకవేళ ((*tmp = *src) != 0)
			src++;
		tmp++;
		లెక్క--;
	}
	తిరిగి dest;
}

అక్షరం *sbi_strchr(స్థిరరాశి అక్షరం *s, పూర్ణాంకం c)
{
	చేయి {
		ఒకవేళ (*s == (అక్షరం)c)
			తిరిగి (అక్షరం *)s;
	} అయితే (*s++ != '\0');

	తిరిగి ఖాళీ;
}

అక్షరం *sbi_strrchr(స్థిరరాశి అక్షరం *s, పూర్ణాంకం c)
{
	స్థిరరాశి అక్షరం *చివరి = s + sbi_strlen(s);

	అయితే (చివరి > s && *చివరి != (అక్షరం)c)
		చివరి--;

	ఒకవేళ (*చివరి != (అక్షరం)c)
		తిరిగి ఖాళీ;
	లేకపోతే
		తిరిగి (అక్షరం *)చివరి;
}
శూన్య *sbi_memset(శూన్య *s, పూర్ణాంకం c, size_t లెక్క)
{
	అక్షరం *తాత్కాలిక = s;

	అయితే (లెక్క > 0) {
		లెక్క--;
		*తాత్కాలిక++ = c;
	}

	తిరిగి s;
}

శూన్య *sbi_memcpy(శూన్య *dest, స్థిరరాశి శూన్య *src, size_t లెక్క)
{
	అక్షరం *temp1	  = dest;
	స్థిరరాశి అక్షరం *temp2 = src;

	అయితే (లెక్క > 0) {
		*temp1++ = *temp2++;
		లెక్క--;
	}

	తిరిగి dest;
}

శూన్య *sbi_memmove(శూన్య *dest, స్థిరరాశి శూన్య *src, size_t లెక్క)
{
	అక్షరం *temp1	  = (అక్షరం *)dest;
	స్థిరరాశి అక్షరం *temp2 = (అక్షరం *)src;

	ఒకవేళ (src == dest)
		తిరిగి dest;

	ఒకవేళ (dest < src) {
		అయితే (లెక్క > 0) {
			*temp1++ = *temp2++;
			లెక్క--;
		}
	} లేకపోతే {
		temp1 = (అక్షరం *)dest + లెక్క - 1;
		temp2 = (అక్షరం *)src + లెక్క - 1;

		అయితే (లెక్క > 0) {
			*temp1-- = *temp2--;
			లెక్క--;
		}
	}

	తిరిగి dest;
}

పూర్ణాంకం sbi_memcmp(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2, size_t లెక్క)
{
	స్థిరరాశి అక్షరం *temp1 = s1;
	స్థిరరాశి అక్షరం *temp2 = s2;

	కొరకు (; లెక్క > 0 && (*temp1 == *temp2); లెక్క--) {
		temp1++;
		temp2++;
	}

	ఒకవేళ (లెక్క > 0)
		తిరిగి *(గుర్తులేని అక్షరం *)temp1 - *(గుర్తులేని అక్షరం *)temp2;
	లేకపోతే
		తిరిగి 0;
}

శూన్య *sbi_memchr(స్థిరరాశి శూన్య *s, పూర్ణాంకం c, size_t లెక్క)
{
	స్థిరరాశి గుర్తులేని అక్షరం *తాత్కాలిక = s;

	అయితే (లెక్క > 0) {
		ఒకవేళ ((గుర్తులేని అక్షరం)c == *తాత్కాలిక++) {
			తిరిగి (శూన్య *)(తాత్కాలిక - 1);
		}
		లెక్క--;
	}

	తిరిగి ఖాళీ;
}
