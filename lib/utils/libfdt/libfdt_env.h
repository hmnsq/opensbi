/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#که_نه_تعریف_شوی LIBFDT_ENV_H
#تعریف_کړه LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_byteorder.h>

#تعریف_کړه لوی_عدد		((صحيح)(~0U >> 1))
#تعریف_کړه UINT_MAX	((بینښه صحيح)~0U)
#تعریف_کړه INT32_MAX	لوی_عدد
#تعریف_کړه UINT32_MAX	UINT_MAX

#که_تعریف_شوی __CHECKER__
#تعریف_کړه FDT_FORCE __attribute__((force))
#تعریف_کړه FDT_BITWISE __attribute__((bitwise))
#وإلا
#تعریف_کړه FDT_FORCE
#تعریف_کړه FDT_BITWISE
#د_که_پای

#تعریف_کړه memmove		sbi_memmove
#تعریف_کړه حافظه_نقل		sbi_memcpy
#تعریف_کړه memcmp		sbi_memcmp
#تعریف_کړه memchr		sbi_memchr
#تعریف_کړه memset		sbi_memset
#تعریف_کړه strchr		sbi_strchr
#تعریف_کړه strrchr		sbi_strrchr
#تعریف_کړه strcpy		sbi_strcpy
#تعریف_کړه strncpy		sbi_strncpy
#تعریف_کړه strcmp		sbi_strcmp
#تعریف_کړه مزي_پرتله		sbi_strncmp
#تعریف_کړه مزي_اوږدوالی		sbi_strlen
#تعریف_کړه strnlen		sbi_strnlen

تعريفنوع be16_t FDT_BITWISE fdt16_t;
تعريفنوع be32_t FDT_BITWISE fdt32_t;
تعريفنوع be64_t FDT_BITWISE fdt64_t;

ساكن مضمن uint16_t fdt16_منزل_cpu(fdt16_t x)
{
	أرجع (FDT_FORCE uint16_t)be16_منزل_cpu(x);
}

ساكن مضمن fdt16_t cpu_منزل_fdt16(uint16_t x)
{
	أرجع (FDT_FORCE fdt16_t)cpu_منزل_be16(x);
}

ساكن مضمن uint32_t fdt32_منزل_cpu(fdt32_t x)
{
	أرجع (FDT_FORCE uint32_t)be32_منزل_cpu(x);
}

ساكن مضمن fdt32_t cpu_منزل_fdt32(uint32_t x)
{
	أرجع (FDT_FORCE fdt32_t)cpu_منزل_be32(x);
}

ساكن مضمن uint64_t fdt64_منزل_cpu(fdt64_t x)
{
	أرجع (FDT_FORCE uint64_t)be64_منزل_cpu(x);
}

ساكن مضمن fdt64_t cpu_منزل_fdt64(uint64_t x)
{
	أرجع (FDT_FORCE fdt64_t)cpu_منزل_be64(x);
}

#که_تعریف_شوی __APPLE__
#ضمن <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# إذا !defined(MAC_OS_X_نسخه_10_7) || (MAC_OS_X_نسخه_MAX_ALLOWED < \
                                         MAC_OS_X_نسخه_10_7)

#تعریف_کړه strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ساكن مضمن size_t fdt_strnlen(ثابت حرف *string, size_t max_شمېر)
{
    ثابت حرف *p = memchr(string, 0, max_شمېر);
    أرجع p ? p - string : max_شمېر;
}

#د_که_پای /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#د_که_پای /* __APPLE__ */

#د_که_پای /* LIBFDT_ENV_H */
