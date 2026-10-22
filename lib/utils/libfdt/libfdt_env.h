/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#جے_بنا_تعریف LIBFDT_ENV_H
#تعریف_کرو LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_byteorder.h>

#تعریف_کرو ودھ_توں_ودھ_عدد		((صحيح)(~0U >> 1))
#تعریف_کرو UINT_MAX	((بنانشانی صحيح)~0U)
#تعریف_کرو INT32_MAX	ودھ_توں_ودھ_عدد
#تعریف_کرو UINT32_MAX	UINT_MAX

#جے_تعریف_شدہ __CHECKER__
#تعریف_کرو FDT_FORCE __attribute__((force))
#تعریف_کرو FDT_BITWISE __attribute__((bitwise))
#وإلا
#تعریف_کرو FDT_FORCE
#تعریف_کرو FDT_BITWISE
#جے_انت

#تعریف_کرو memmove		sbi_memmove
#تعریف_کرو حافظہ_نقل		sbi_memcpy
#تعریف_کرو memcmp		sbi_memcmp
#تعریف_کرو memchr		sbi_memchr
#تعریف_کرو memset		sbi_memset
#تعریف_کرو strchr		sbi_strchr
#تعریف_کرو strrchr		sbi_strrchr
#تعریف_کرو strcpy		sbi_strcpy
#تعریف_کرو strncpy		sbi_strncpy
#تعریف_کرو strcmp		sbi_strcmp
#تعریف_کرو لڑی_مقابلہ		sbi_strncmp
#تعریف_کرو لڑی_لمبائی		sbi_strlen
#تعریف_کرو strnlen		sbi_strnlen

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

#جے_تعریف_شدہ __APPLE__
#ضمن <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# إذا !defined(MAC_OS_X_نسخہ_10_7) || (MAC_OS_X_نسخہ_MAX_ALLOWED < \
                                         MAC_OS_X_نسخہ_10_7)

#تعریف_کرو strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ساكن مضمن size_t fdt_strnlen(ثابت حرف *string, size_t max_گنتی)
{
    ثابت حرف *p = memchr(string, 0, max_گنتی);
    أرجع p ? p - string : max_گنتی;
}

#جے_انت /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#جے_انت /* __APPLE__ */

#جے_انت /* LIBFDT_ENV_H */
