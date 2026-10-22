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

पूर्णाङ्क sbi_strncmp(ध्रुव वर्ण *a, ध्रुव वर्ण *b, size_t गिनती)
{
	/* search first diff or end of string */
	प्रति (; गिनती > 0 && *a == *b && *a != '\0'; a++, b++, गिनती--)
		;

	/* No difference till the end */
	यदि (!गिनती)
		प्रत्यागम 0;

	प्रत्यागम *a - *b;
}

size_t sbi_strlen(ध्रुव वर्ण *str)
{
	unsigned long ret = 0;

	यावत् (*str != '\0') {
		ret++;
		str++;
	}

	प्रत्यागम ret;
}

size_t sbi_strnlen(ध्रुव वर्ण *str, size_t गिनती)
{
	unsigned long ret = 0;

	यावत् (*str != '\0' && ret < गिनती) {
		ret++;
		str++;
	}

	प्रत्यागम ret;
}

वर्ण *sbi_strcpy(वर्ण *dest, ध्रुव वर्ण *src)
{
	वर्ण *ret = dest;

	यावत् ((*dest++ = *src++) != '\0') {
	}
	प्रत्यागम ret;
}

वर्ण *sbi_strncpy(वर्ण *dest, ध्रुव वर्ण *src, size_t गिनती)
{
	वर्ण *tmp = dest;

	यावत् (गिनती) {
		यदि ((*tmp = *src) != 0)
			src++;
		tmp++;
		गिनती--;
	}
	प्रत्यागम dest;
}

वर्ण *sbi_strchr(ध्रुव वर्ण *s, पूर्णाङ्क c)
{
	कुरु {
		यदि (*s == (वर्ण)c)
			प्रत्यागम (वर्ण *)s;
	} यावत् (*s++ != '\0');

	प्रत्यागम रिक्त;
}

वर्ण *sbi_strrchr(ध्रुव वर्ण *s, पूर्णाङ्क c)
{
	ध्रुव वर्ण *last = s + sbi_strlen(s);

	यावत् (last > s && *last != (वर्ण)c)
		last--;

	यदि (*last != (वर्ण)c)
		प्रत्यागम रिक्त;
	अन्यथा
		प्रत्यागम (वर्ण *)last;
}
शून्य *sbi_memset(शून्य *s, पूर्णाङ्क c, size_t गिनती)
{
	वर्ण *temp = s;

	यावत् (गिनती > 0) {
		गिनती--;
		*temp++ = c;
	}

	प्रत्यागम s;
}

शून्य *sbi_memcpy(शून्य *dest, ध्रुव शून्य *src, size_t गिनती)
{
	वर्ण *temp1	  = dest;
	ध्रुव वर्ण *temp2 = src;

	यावत् (गिनती > 0) {
		*temp1++ = *temp2++;
		गिनती--;
	}

	प्रत्यागम dest;
}

शून्य *sbi_memmove(शून्य *dest, ध्रुव शून्य *src, size_t गिनती)
{
	वर्ण *temp1	  = (वर्ण *)dest;
	ध्रुव वर्ण *temp2 = (वर्ण *)src;

	यदि (src == dest)
		प्रत्यागम dest;

	यदि (dest < src) {
		यावत् (गिनती > 0) {
			*temp1++ = *temp2++;
			गिनती--;
		}
	} अन्यथा {
		temp1 = (वर्ण *)dest + गिनती - 1;
		temp2 = (वर्ण *)src + गिनती - 1;

		यावत् (गिनती > 0) {
			*temp1-- = *temp2--;
			गिनती--;
		}
	}

	प्रत्यागम dest;
}

पूर्णाङ्क sbi_memcmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2, size_t गिनती)
{
	ध्रुव वर्ण *temp1 = s1;
	ध्रुव वर्ण *temp2 = s2;

	प्रति (; गिनती > 0 && (*temp1 == *temp2); गिनती--) {
		temp1++;
		temp2++;
	}

	यदि (गिनती > 0)
		प्रत्यागम *(unsigned वर्ण *)temp1 - *(unsigned वर्ण *)temp2;
	अन्यथा
		प्रत्यागम 0;
}

शून्य *sbi_memchr(ध्रुव शून्य *s, पूर्णाङ्क c, size_t गिनती)
{
	ध्रुव unsigned वर्ण *temp = s;

	यावत् (गिनती > 0) {
		यदि ((unsigned वर्ण)c == *temp++) {
			प्रत्यागम (शून्य *)(temp - 1);
		}
		गिनती--;
	}

	प्रत्यागम रिक्त;
}
