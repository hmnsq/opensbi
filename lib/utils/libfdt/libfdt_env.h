/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#جيڪڏهن_وصف_نه_ٿيل LIBFDT_ENV_H
#وصف_ڪريو LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_byteorder.h>

#وصف_ڪريو وڌ_کان_وڌ_عدد		((صحيح)(~0U >> 1))
#وصف_ڪريو UINT_MAX	((بنانشان صحيح)~0U)
#وصف_ڪريو INT32_MAX	وڌ_کان_وڌ_عدد
#وصف_ڪريو UINT32_MAX	UINT_MAX

#جيڪڏهن_وصف_ٿيل __CHECKER__
#وصف_ڪريو FDT_FORCE __attribute__((force))
#وصف_ڪريو FDT_BITWISE __attribute__((bitwise))
#وإلا
#وصف_ڪريو FDT_FORCE
#وصف_ڪريو FDT_BITWISE
#جيڪڏهن_پورو

#وصف_ڪريو memmove		sbi_memmove
#وصف_ڪريو يادگيري_نقل		sbi_memcpy
#وصف_ڪريو memcmp		sbi_memcmp
#وصف_ڪريو memchr		sbi_memchr
#وصف_ڪريو memset		sbi_memset
#وصف_ڪريو strchr		sbi_strchr
#وصف_ڪريو strrchr		sbi_strrchr
#وصف_ڪريو strcpy		sbi_strcpy
#وصف_ڪريو strncpy		sbi_strncpy
#وصف_ڪريو strcmp		sbi_strcmp
#وصف_ڪريو لڙي_ڀيٽ		sbi_strncmp
#وصف_ڪريو لڙي_ڊيگهه		sbi_strlen
#وصف_ڪريو strnlen		sbi_strnlen

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

#جيڪڏهن_وصف_ٿيل __APPLE__
#ضمن <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# إذا !defined(MAC_OS_X_نسخو_10_7) || (MAC_OS_X_نسخو_MAX_ALLOWED < \
                                         MAC_OS_X_نسخو_10_7)

#وصف_ڪريو strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ساكن مضمن size_t fdt_strnlen(ثابت حرف *string, size_t max_ڳڻپ)
{
    ثابت حرف *p = memchr(string, 0, max_ڳڻپ);
    أرجع p ? p - string : max_ڳڻپ;
}

#جيڪڏهن_پورو /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#جيڪڏهن_پورو /* __APPLE__ */

#جيڪڏهن_پورو /* LIBFDT_ENV_H */
