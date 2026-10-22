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

#समावेश <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
पूर्णांक sbi_strcmp(स्थिर अक्षर *a, स्थिर अक्षर *b)
{
	/* search first diff or end of string */
	क्रम (; *a == *b && *a != '\0'; a++, b++)
		;

	वापस *a - *b;
}

पूर्णांक sbi_strncmp(स्थिर अक्षर *a, स्थिर अक्षर *b, माप_प्रकार गिनती)
{
	/* search first diff or end of string */
	क्रम (; गिनती > 0 && *a == *b && *a != '\0'; a++, b++, गिनती--)
		;

	/* No difference till the end */
	अगर (!गिनती)
		वापस 0;

	वापस *a - *b;
}

माप_प्रकार sbi_strlen(स्थिर अक्षर *str)
{
	अचिन्हित दीर्घ ret = 0;

	जबतक (*str != '\0') {
		ret++;
		str++;
	}

	वापस ret;
}

माप_प्रकार sbi_strnlen(स्थिर अक्षर *str, माप_प्रकार गिनती)
{
	अचिन्हित दीर्घ ret = 0;

	जबतक (*str != '\0' && ret < गिनती) {
		ret++;
		str++;
	}

	वापस ret;
}

अक्षर *sbi_strcpy(अक्षर *dest, स्थिर अक्षर *src)
{
	अक्षर *ret = dest;

	जबतक ((*dest++ = *src++) != '\0') {
	}
	वापस ret;
}

अक्षर *sbi_strncpy(अक्षर *dest, स्थिर अक्षर *src, माप_प्रकार गिनती)
{
	अक्षर *tmp = dest;

	जबतक (गिनती) {
		अगर ((*tmp = *src) != 0)
			src++;
		tmp++;
		गिनती--;
	}
	वापस dest;
}

अक्षर *sbi_strchr(स्थिर अक्षर *s, पूर्णांक c)
{
	करो {
		अगर (*s == (अक्षर)c)
			वापस (अक्षर *)s;
	} जबतक (*s++ != '\0');

	वापस शून्य;
}

अक्षर *sbi_strrchr(स्थिर अक्षर *s, पूर्णांक c)
{
	स्थिर अक्षर *last = s + sbi_strlen(s);

	जबतक (last > s && *last != (अक्षर)c)
		last--;

	अगर (*last != (अक्षर)c)
		वापस शून्य;
	अन्यथा
		वापस (अक्षर *)last;
}
व्योम *sbi_memset(व्योम *s, पूर्णांक c, माप_प्रकार गिनती)
{
	अक्षर *अस्थायी = s;

	जबतक (गिनती > 0) {
		गिनती--;
		*अस्थायी++ = c;
	}

	वापस s;
}

व्योम *sbi_memcpy(व्योम *dest, स्थिर व्योम *src, माप_प्रकार गिनती)
{
	अक्षर *temp1	  = dest;
	स्थिर अक्षर *temp2 = src;

	जबतक (गिनती > 0) {
		*temp1++ = *temp2++;
		गिनती--;
	}

	वापस dest;
}

व्योम *sbi_memmove(व्योम *dest, स्थिर व्योम *src, माप_प्रकार गिनती)
{
	अक्षर *temp1	  = (अक्षर *)dest;
	स्थिर अक्षर *temp2 = (अक्षर *)src;

	अगर (src == dest)
		वापस dest;

	अगर (dest < src) {
		जबतक (गिनती > 0) {
			*temp1++ = *temp2++;
			गिनती--;
		}
	} अन्यथा {
		temp1 = (अक्षर *)dest + गिनती - 1;
		temp2 = (अक्षर *)src + गिनती - 1;

		जबतक (गिनती > 0) {
			*temp1-- = *temp2--;
			गिनती--;
		}
	}

	वापस dest;
}

पूर्णांक sbi_memcmp(स्थिर व्योम *s1, स्थिर व्योम *s2, माप_प्रकार गिनती)
{
	स्थिर अक्षर *temp1 = s1;
	स्थिर अक्षर *temp2 = s2;

	क्रम (; गिनती > 0 && (*temp1 == *temp2); गिनती--) {
		temp1++;
		temp2++;
	}

	अगर (गिनती > 0)
		वापस *(अचिन्हित अक्षर *)temp1 - *(अचिन्हित अक्षर *)temp2;
	अन्यथा
		वापस 0;
}

व्योम *sbi_memchr(स्थिर व्योम *s, पूर्णांक c, माप_प्रकार गिनती)
{
	स्थिर अचिन्हित अक्षर *अस्थायी = s;

	जबतक (गिनती > 0) {
		अगर ((अचिन्हित अक्षर)c == *अस्थायी++) {
			वापस (व्योम *)(अस्थायी - 1);
		}
		गिनती--;
	}

	वापस शून्य;
}
