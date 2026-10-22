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

#അന്തര്ഭാവ <sbi/sbi_string.h>

/*
  Provides sbi_strcmp for the completeness of supporting string functions.
  it is not recommended to use sbi_strcmp() but use sbi_strncmp instead.
*/
പൂർണ്ണസംഖ്യ sbi_strcmp(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b)
{
	/* search first diff or end of string */
	വേണ്ടി (; *a == *b && *a != '\0'; a++, b++)
		;

	തിരികെ *a - *b;
}

പൂർണ്ണസംഖ്യ sbi_strncmp(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, size_t എണ്ണം)
{
	/* search first diff or end of string */
	വേണ്ടി (; എണ്ണം > 0 && *a == *b && *a != '\0'; a++, b++, എണ്ണം--)
		;

	/* No difference till the end */
	എങ്കിൽ (!എണ്ണം)
		തിരികെ 0;

	തിരികെ *a - *b;
}

size_t sbi_strlen(സ്ഥിരം അക്ഷരം *str)
{
	unsigned long ret = 0;

	ആകുമ്പോൾ (*str != '\0') {
		ret++;
		str++;
	}

	തിരികെ ret;
}

size_t sbi_strnlen(സ്ഥിരം അക്ഷരം *str, size_t എണ്ണം)
{
	unsigned long ret = 0;

	ആകുമ്പോൾ (*str != '\0' && ret < എണ്ണം) {
		ret++;
		str++;
	}

	തിരികെ ret;
}

അക്ഷരം *sbi_strcpy(അക്ഷരം *dest, സ്ഥിരം അക്ഷരം *src)
{
	അക്ഷരം *ret = dest;

	ആകുമ്പോൾ ((*dest++ = *src++) != '\0') {
	}
	തിരികെ ret;
}

അക്ഷരം *sbi_strncpy(അക്ഷരം *dest, സ്ഥിരം അക്ഷരം *src, size_t എണ്ണം)
{
	അക്ഷരം *tmp = dest;

	ആകുമ്പോൾ (എണ്ണം) {
		എങ്കിൽ ((*tmp = *src) != 0)
			src++;
		tmp++;
		എണ്ണം--;
	}
	തിരികെ dest;
}

അക്ഷരം *sbi_strchr(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ c)
{
	ചെയ്യുക {
		എങ്കിൽ (*s == (അക്ഷരം)c)
			തിരികെ (അക്ഷരം *)s;
	} ആകുമ്പോൾ (*s++ != '\0');

	തിരികെ ശൂന്യ;
}

അക്ഷരം *sbi_strrchr(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ c)
{
	സ്ഥിരം അക്ഷരം *last = s + sbi_strlen(s);

	ആകുമ്പോൾ (last > s && *last != (അക്ഷരം)c)
		last--;

	എങ്കിൽ (*last != (അക്ഷരം)c)
		തിരികെ ശൂന്യ;
	അല്ലെങ്കിൽ
		തിരികെ (അക്ഷരം *)last;
}
ശൂന്യം *sbi_memset(ശൂന്യം *s, പൂർണ്ണസംഖ്യ c, size_t എണ്ണം)
{
	അക്ഷരം *temp = s;

	ആകുമ്പോൾ (എണ്ണം > 0) {
		എണ്ണം--;
		*temp++ = c;
	}

	തിരികെ s;
}

ശൂന്യം *sbi_memcpy(ശൂന്യം *dest, സ്ഥിരം ശൂന്യം *src, size_t എണ്ണം)
{
	അക്ഷരം *temp1	  = dest;
	സ്ഥിരം അക്ഷരം *temp2 = src;

	ആകുമ്പോൾ (എണ്ണം > 0) {
		*temp1++ = *temp2++;
		എണ്ണം--;
	}

	തിരികെ dest;
}

ശൂന്യം *sbi_memmove(ശൂന്യം *dest, സ്ഥിരം ശൂന്യം *src, size_t എണ്ണം)
{
	അക്ഷരം *temp1	  = (അക്ഷരം *)dest;
	സ്ഥിരം അക്ഷരം *temp2 = (അക്ഷരം *)src;

	എങ്കിൽ (src == dest)
		തിരികെ dest;

	എങ്കിൽ (dest < src) {
		ആകുമ്പോൾ (എണ്ണം > 0) {
			*temp1++ = *temp2++;
			എണ്ണം--;
		}
	} അല്ലെങ്കിൽ {
		temp1 = (അക്ഷരം *)dest + എണ്ണം - 1;
		temp2 = (അക്ഷരം *)src + എണ്ണം - 1;

		ആകുമ്പോൾ (എണ്ണം > 0) {
			*temp1-- = *temp2--;
			എണ്ണം--;
		}
	}

	തിരികെ dest;
}

പൂർണ്ണസംഖ്യ sbi_memcmp(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2, size_t എണ്ണം)
{
	സ്ഥിരം അക്ഷരം *temp1 = s1;
	സ്ഥിരം അക്ഷരം *temp2 = s2;

	വേണ്ടി (; എണ്ണം > 0 && (*temp1 == *temp2); എണ്ണം--) {
		temp1++;
		temp2++;
	}

	എങ്കിൽ (എണ്ണം > 0)
		തിരികെ *(unsigned അക്ഷരം *)temp1 - *(unsigned അക്ഷരം *)temp2;
	അല്ലെങ്കിൽ
		തിരികെ 0;
}

ശൂന്യം *sbi_memchr(സ്ഥിരം ശൂന്യം *s, പൂർണ്ണസംഖ്യ c, size_t എണ്ണം)
{
	സ്ഥിരം unsigned അക്ഷരം *temp = s;

	ആകുമ്പോൾ (എണ്ണം > 0) {
		എങ്കിൽ ((unsigned അക്ഷരം)c == *temp++) {
			തിരികെ (ശൂന്യം *)(temp - 1);
		}
		എണ്ണം--;
	}

	തിരികെ ശൂന്യ;
}
