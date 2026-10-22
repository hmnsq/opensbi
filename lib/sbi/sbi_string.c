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
पूर्णाङ्क sbi_strcmp(अचर वर्ण *a, अचर वर्ण *b)
{
	/* search first diff or end of string */
	लागि (; *a == *b && *a != '\0'; a++, b++)
		;

	फर्काऊ *a - *b;
}

पूर्णाङ्क sbi_strncmp(अचर वर्ण *a, अचर वर्ण *b, माप_प्रकार गन्ती)
{
	/* search first diff or end of string */
	लागि (; गन्ती > 0 && *a == *b && *a != '\0'; a++, b++, गन्ती--)
		;

	/* No difference till the end */
	यदि (!गन्ती)
		फर्काऊ 0;

	फर्काऊ *a - *b;
}

माप_प्रकार sbi_strlen(अचर वर्ण *str)
{
	अचिन्हित दीर्घ ret = 0;

	जबसम्म (*str != '\0') {
		ret++;
		str++;
	}

	फर्काऊ ret;
}

माप_प्रकार sbi_strnlen(अचर वर्ण *str, माप_प्रकार गन्ती)
{
	अचिन्हित दीर्घ ret = 0;

	जबसम्म (*str != '\0' && ret < गन्ती) {
		ret++;
		str++;
	}

	फर्काऊ ret;
}

वर्ण *sbi_strcpy(वर्ण *dest, अचर वर्ण *src)
{
	वर्ण *ret = dest;

	जबसम्म ((*dest++ = *src++) != '\0') {
	}
	फर्काऊ ret;
}

वर्ण *sbi_strncpy(वर्ण *dest, अचर वर्ण *src, माप_प्रकार गन्ती)
{
	वर्ण *tmp = dest;

	जबसम्म (गन्ती) {
		यदि ((*tmp = *src) != 0)
			src++;
		tmp++;
		गन्ती--;
	}
	फर्काऊ dest;
}

वर्ण *sbi_strchr(अचर वर्ण *s, पूर्णाङ्क c)
{
	गर {
		यदि (*s == (वर्ण)c)
			फर्काऊ (वर्ण *)s;
	} जबसम्म (*s++ != '\0');

	फर्काऊ रिक्त;
}

वर्ण *sbi_strrchr(अचर वर्ण *s, पूर्णाङ्क c)
{
	अचर वर्ण *last = s + sbi_strlen(s);

	जबसम्म (last > s && *last != (वर्ण)c)
		last--;

	यदि (*last != (वर्ण)c)
		फर्काऊ रिक्त;
	अन्यथा
		फर्काऊ (वर्ण *)last;
}
शून्य *sbi_memset(शून्य *s, पूर्णाङ्क c, माप_प्रकार गन्ती)
{
	वर्ण *temp = s;

	जबसम्म (गन्ती > 0) {
		गन्ती--;
		*temp++ = c;
	}

	फर्काऊ s;
}

शून्य *sbi_memcpy(शून्य *dest, अचर शून्य *src, माप_प्रकार गन्ती)
{
	वर्ण *temp1	  = dest;
	अचर वर्ण *temp2 = src;

	जबसम्म (गन्ती > 0) {
		*temp1++ = *temp2++;
		गन्ती--;
	}

	फर्काऊ dest;
}

शून्य *sbi_memmove(शून्य *dest, अचर शून्य *src, माप_प्रकार गन्ती)
{
	वर्ण *temp1	  = (वर्ण *)dest;
	अचर वर्ण *temp2 = (वर्ण *)src;

	यदि (src == dest)
		फर्काऊ dest;

	यदि (dest < src) {
		जबसम्म (गन्ती > 0) {
			*temp1++ = *temp2++;
			गन्ती--;
		}
	} अन्यथा {
		temp1 = (वर्ण *)dest + गन्ती - 1;
		temp2 = (वर्ण *)src + गन्ती - 1;

		जबसम्म (गन्ती > 0) {
			*temp1-- = *temp2--;
			गन्ती--;
		}
	}

	फर्काऊ dest;
}

पूर्णाङ्क sbi_memcmp(अचर शून्य *s1, अचर शून्य *s2, माप_प्रकार गन्ती)
{
	अचर वर्ण *temp1 = s1;
	अचर वर्ण *temp2 = s2;

	लागि (; गन्ती > 0 && (*temp1 == *temp2); गन्ती--) {
		temp1++;
		temp2++;
	}

	यदि (गन्ती > 0)
		फर्काऊ *(अचिन्हित वर्ण *)temp1 - *(अचिन्हित वर्ण *)temp2;
	अन्यथा
		फर्काऊ 0;
}

शून्य *sbi_memchr(अचर शून्य *s, पूर्णाङ्क c, माप_प्रकार गन्ती)
{
	अचर अचिन्हित वर्ण *temp = s;

	जबसम्म (गन्ती > 0) {
		यदि ((अचिन्हित वर्ण)c == *temp++) {
			फर्काऊ (शून्य *)(temp - 1);
		}
		गन्ती--;
	}

	फर्काऊ रिक्त;
}
