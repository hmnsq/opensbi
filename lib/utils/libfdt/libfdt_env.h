/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#اگر_تعریف_نشده LIBFDT_ENV_H
#تعریف_کن LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_byteorder.h>

#تعریف_کن بیشترین_صحیح		((صحيح)(~0U >> 1))
#تعریف_کن UINT_MAX	((بیعلامت صحيح)~0U)
#تعریف_کن INT32_MAX	بیشترین_صحیح
#تعریف_کن UINT32_MAX	UINT_MAX

#اگر_تعریف_شده __CHECKER__
#تعریف_کن FDT_FORCE __attribute__((force))
#تعریف_کن FDT_BITWISE __attribute__((bitwise))
#وإلا
#تعریف_کن FDT_FORCE
#تعریف_کن FDT_BITWISE
#پایان_اگر

#تعریف_کن memmove		sbi_memmove
#تعریف_کن کپی_حافظه		sbi_memcpy
#تعریف_کن memcmp		sbi_memcmp
#تعریف_کن memchr		sbi_memchr
#تعریف_کن memset		sbi_memset
#تعریف_کن strchr		sbi_strchr
#تعریف_کن strrchr		sbi_strrchr
#تعریف_کن strcpy		sbi_strcpy
#تعریف_کن strncpy		sbi_strncpy
#تعریف_کن strcmp		sbi_strcmp
#تعریف_کن مقایسه_رشته		sbi_strncmp
#تعریف_کن طول_رشته		sbi_strlen
#تعریف_کن strnlen		sbi_strnlen

تعريفنوع be16_t FDT_BITWISE fdt16_t;
تعريفنوع be32_t FDT_BITWISE fdt32_t;
تعريفنوع be64_t FDT_BITWISE fdt64_t;

ساكن مضمن uint16_t fdt16_مقصد_cpu(fdt16_t x)
{
	أرجع (FDT_FORCE uint16_t)be16_مقصد_cpu(x);
}

ساكن مضمن fdt16_t cpu_مقصد_fdt16(uint16_t x)
{
	أرجع (FDT_FORCE fdt16_t)cpu_مقصد_be16(x);
}

ساكن مضمن uint32_t fdt32_مقصد_cpu(fdt32_t x)
{
	أرجع (FDT_FORCE uint32_t)be32_مقصد_cpu(x);
}

ساكن مضمن fdt32_t cpu_مقصد_fdt32(uint32_t x)
{
	أرجع (FDT_FORCE fdt32_t)cpu_مقصد_be32(x);
}

ساكن مضمن uint64_t fdt64_مقصد_cpu(fdt64_t x)
{
	أرجع (FDT_FORCE uint64_t)be64_مقصد_cpu(x);
}

ساكن مضمن fdt64_t cpu_مقصد_fdt64(uint64_t x)
{
	أرجع (FDT_FORCE fdt64_t)cpu_مقصد_be64(x);
}

#اگر_تعریف_شده __APPLE__
#ضمن <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# إذا !defined(MAC_OS_X_نسخه_10_7) || (MAC_OS_X_نسخه_MAX_ALLOWED < \
                                         MAC_OS_X_نسخه_10_7)

#تعریف_کن strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ساكن مضمن size_t fdt_strnlen(ثابت حرف *string, size_t max_شمار)
{
    ثابت حرف *p = memchr(string, 0, max_شمار);
    أرجع p ? p - string : max_شمار;
}

#پایان_اگر /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#پایان_اگر /* __APPLE__ */

#پایان_اگر /* LIBFDT_ENV_H */
