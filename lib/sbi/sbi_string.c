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

#अन्तर्भाव <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
पूर्णाङ्क sbi_strcmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b)
{
	/* search first diff or end of string */
	प्रति (; *a == *b && *a != '\0'; a++, b++)
		;

	प्रत्यागम *a - *b;
}

पूर्णाङ्क sbi_strncmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b, size_t गणनम्)
{
	/* search first diff or end of string */
	प्रति (; गणनम् > 0 && *a == *b && *a != '\0'; a++, b++, गणनम्--)
		;

	/* No difference till the end */
	जइ (!गणनम्)
		प्रत्यागम 0;

	प्रत्यागम *a - *b;
}

size_t sbi_strlen(ध्रुव वर्ण *str)
{
	unsigned long ret = 0;

	जाव (*str != '\0') {
		ret++;
		str++;
	}

	प्रत्यागम ret;
}

size_t sbi_strnlen(ध्रुव वर्ण *str, size_t गणनम्)
{
	unsigned long ret = 0;

	जाव (*str != '\0' && ret < गणनम्) {
		ret++;
		str++;
	}

	प्रत्यागम ret;
}

वर्ण *sbi_strcpy(वर्ण *dest, ध्रुव वर्ण *src)
{
	वर्ण *ret = dest;

	जाव ((*dest++ = *src++) != '\0') {
	}
	प्रत्यागम ret;
}

वर्ण *sbi_strncpy(वर्ण *dest, ध्रुव वर्ण *src, size_t गणनम्)
{
	वर्ण *tmp = dest;

	जाव (गणनम्) {
		जइ ((*tmp = *src) != 0)
			src++;
		tmp++;
		गणनम्--;
	}
	प्रत्यागम dest;
}

वर्ण *sbi_strchr(ध्रुव वर्ण *s, पूर्णाङ्क c)
{
	करेहि {
		जइ (*s == (वर्ण)c)
			प्रत्यागम (वर्ण *)s;
	} जाव (*s++ != '\0');

	प्रत्यागम रिक्त;
}

वर्ण *sbi_strrchr(ध्रुव वर्ण *s, पूर्णाङ्क c)
{
	ध्रुव वर्ण *last = s + sbi_strlen(s);

	जाव (last > s && *last != (वर्ण)c)
		last--;

	जइ (*last != (वर्ण)c)
		प्रत्यागम रिक्त;
	अण्णहा
		प्रत्यागम (वर्ण *)last;
}
शून्य *sbi_memset(शून्य *s, पूर्णाङ्क c, size_t गणनम्)
{
	वर्ण *temp = s;

	जाव (गणनम् > 0) {
		गणनम्--;
		*temp++ = c;
	}

	प्रत्यागम s;
}

शून्य *sbi_memcpy(शून्य *dest, ध्रुव शून्य *src, size_t गणनम्)
{
	वर्ण *temp1	  = dest;
	ध्रुव वर्ण *temp2 = src;

	जाव (गणनम् > 0) {
		*temp1++ = *temp2++;
		गणनम्--;
	}

	प्रत्यागम dest;
}

शून्य *sbi_memmove(शून्य *dest, ध्रुव शून्य *src, size_t गणनम्)
{
	वर्ण *temp1	  = (वर्ण *)dest;
	ध्रुव वर्ण *temp2 = (वर्ण *)src;

	जइ (src == dest)
		प्रत्यागम dest;

	जइ (dest < src) {
		जाव (गणनम् > 0) {
			*temp1++ = *temp2++;
			गणनम्--;
		}
	} अण्णहा {
		temp1 = (वर्ण *)dest + गणनम् - 1;
		temp2 = (वर्ण *)src + गणनम् - 1;

		जाव (गणनम् > 0) {
			*temp1-- = *temp2--;
			गणनम्--;
		}
	}

	प्रत्यागम dest;
}

पूर्णाङ्क sbi_memcmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2, size_t गणनम्)
{
	ध्रुव वर्ण *temp1 = s1;
	ध्रुव वर्ण *temp2 = s2;

	प्रति (; गणनम् > 0 && (*temp1 == *temp2); गणनम्--) {
		temp1++;
		temp2++;
	}

	जइ (गणनम् > 0)
		प्रत्यागम *(unsigned वर्ण *)temp1 - *(unsigned वर्ण *)temp2;
	अण्णहा
		प्रत्यागम 0;
}

शून्य *sbi_memchr(ध्रुव शून्य *s, पूर्णाङ्क c, size_t गणनम्)
{
	ध्रुव unsigned वर्ण *temp = s;

	जाव (गणनम् > 0) {
		जइ ((unsigned वर्ण)c == *temp++) {
			प्रत्यागम (शून्य *)(temp - 1);
		}
		गणनम्--;
	}

	प्रत्यागम रिक्त;
}
