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
पूर्णांक sbi_strcmp(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b)
{
	/* search first diff or end of string */
	साठी (; *a == *b && *a != '\0'; a++, b++)
		;

	परत *a - *b;
}

पूर्णांक sbi_strncmp(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b, माप_प्रकार मोजणी)
{
	/* search first diff or end of string */
	साठी (; मोजणी > 0 && *a == *b && *a != '\0'; a++, b++, मोजणी--)
		;

	/* No difference till the end */
	जर (!मोजणी)
		परत 0;

	परत *a - *b;
}

माप_प्रकार sbi_strlen(स्थिरांक अक्षर *str)
{
	अचिन्हित दीर्घ ret = 0;

	पर्यंत (*str != '\0') {
		ret++;
		str++;
	}

	परत ret;
}

माप_प्रकार sbi_strnlen(स्थिरांक अक्षर *str, माप_प्रकार मोजणी)
{
	अचिन्हित दीर्घ ret = 0;

	पर्यंत (*str != '\0' && ret < मोजणी) {
		ret++;
		str++;
	}

	परत ret;
}

अक्षर *sbi_strcpy(अक्षर *dest, स्थिरांक अक्षर *src)
{
	अक्षर *ret = dest;

	पर्यंत ((*dest++ = *src++) != '\0') {
	}
	परत ret;
}

अक्षर *sbi_strncpy(अक्षर *dest, स्थिरांक अक्षर *src, माप_प्रकार मोजणी)
{
	अक्षर *tmp = dest;

	पर्यंत (मोजणी) {
		जर ((*tmp = *src) != 0)
			src++;
		tmp++;
		मोजणी--;
	}
	परत dest;
}

अक्षर *sbi_strchr(स्थिरांक अक्षर *s, पूर्णांक c)
{
	करा {
		जर (*s == (अक्षर)c)
			परत (अक्षर *)s;
	} पर्यंत (*s++ != '\0');

	परत रिक्त;
}

अक्षर *sbi_strrchr(स्थिरांक अक्षर *s, पूर्णांक c)
{
	स्थिरांक अक्षर *last = s + sbi_strlen(s);

	पर्यंत (last > s && *last != (अक्षर)c)
		last--;

	जर (*last != (अक्षर)c)
		परत रिक्त;
	अन्यथा
		परत (अक्षर *)last;
}
शून्य *sbi_memset(शून्य *s, पूर्णांक c, माप_प्रकार मोजणी)
{
	अक्षर *temp = s;

	पर्यंत (मोजणी > 0) {
		मोजणी--;
		*temp++ = c;
	}

	परत s;
}

शून्य *sbi_memcpy(शून्य *dest, स्थिरांक शून्य *src, माप_प्रकार मोजणी)
{
	अक्षर *temp1	  = dest;
	स्थिरांक अक्षर *temp2 = src;

	पर्यंत (मोजणी > 0) {
		*temp1++ = *temp2++;
		मोजणी--;
	}

	परत dest;
}

शून्य *sbi_memmove(शून्य *dest, स्थिरांक शून्य *src, माप_प्रकार मोजणी)
{
	अक्षर *temp1	  = (अक्षर *)dest;
	स्थिरांक अक्षर *temp2 = (अक्षर *)src;

	जर (src == dest)
		परत dest;

	जर (dest < src) {
		पर्यंत (मोजणी > 0) {
			*temp1++ = *temp2++;
			मोजणी--;
		}
	} अन्यथा {
		temp1 = (अक्षर *)dest + मोजणी - 1;
		temp2 = (अक्षर *)src + मोजणी - 1;

		पर्यंत (मोजणी > 0) {
			*temp1-- = *temp2--;
			मोजणी--;
		}
	}

	परत dest;
}

पूर्णांक sbi_memcmp(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2, माप_प्रकार मोजणी)
{
	स्थिरांक अक्षर *temp1 = s1;
	स्थिरांक अक्षर *temp2 = s2;

	साठी (; मोजणी > 0 && (*temp1 == *temp2); मोजणी--) {
		temp1++;
		temp2++;
	}

	जर (मोजणी > 0)
		परत *(अचिन्हित अक्षर *)temp1 - *(अचिन्हित अक्षर *)temp2;
	अन्यथा
		परत 0;
}

शून्य *sbi_memchr(स्थिरांक शून्य *s, पूर्णांक c, माप_प्रकार मोजणी)
{
	स्थिरांक अचिन्हित अक्षर *temp = s;

	पर्यंत (मोजणी > 0) {
		जर ((अचिन्हित अक्षर)c == *temp++) {
			परत (शून्य *)(temp - 1);
		}
		मोजणी--;
	}

	परत रिक्त;
}
