/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LIBFDT_ENV_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_byteorder.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ		((ਪੂਰਨ)(~0U >> 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT_MAX	((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)~0U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT32_MAX	ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT32_MAX	UINT_MAX

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CHECKER__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_FORCE __attribute__((force))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_BITWISE __attribute__((bitwise))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_FORCE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_BITWISE
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memmove		sbi_memmove
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮ੍ਰਿਤੀ_ਨਕਲ		sbi_memcpy
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memcmp		sbi_memcmp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memchr		sbi_memchr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memset		sbi_memset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strchr		sbi_strchr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strrchr		sbi_strrchr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strcpy		sbi_strcpy
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strncpy		sbi_strncpy
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strcmp		sbi_strcmp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੜੀ_ਤੁਲਨਾ		sbi_strncmp
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੜੀ_ਲੰਬਾਈ		sbi_strlen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strnlen		sbi_strnlen

ਕਿਸਮਪਰਿਭਾਸ਼ਾ be16_t FDT_BITWISE fdt16_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ be32_t FDT_BITWISE fdt32_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ be64_t FDT_BITWISE fdt64_t;

ਸਥਿਰ ਸਤਰਅੰਦਰ uint16_t fdt16_ਮੰਜ਼ਿਲ_cpu(fdt16_t x)
{
	ਵਾਪਸ (FDT_FORCE uint16_t)be16_ਮੰਜ਼ਿਲ_cpu(x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ fdt16_t cpu_ਮੰਜ਼ਿਲ_fdt16(uint16_t x)
{
	ਵਾਪਸ (FDT_FORCE fdt16_t)cpu_ਮੰਜ਼ਿਲ_be16(x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t fdt32_ਮੰਜ਼ਿਲ_cpu(fdt32_t x)
{
	ਵਾਪਸ (FDT_FORCE uint32_t)be32_ਮੰਜ਼ਿਲ_cpu(x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ fdt32_t cpu_ਮੰਜ਼ਿਲ_fdt32(uint32_t x)
{
	ਵਾਪਸ (FDT_FORCE fdt32_t)cpu_ਮੰਜ਼ਿਲ_be32(x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t fdt64_ਮੰਜ਼ਿਲ_cpu(fdt64_t x)
{
	ਵਾਪਸ (FDT_FORCE uint64_t)be64_ਮੰਜ਼ਿਲ_cpu(x);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ fdt64_t cpu_ਮੰਜ਼ਿਲ_fdt64(uint64_t x)
{
	ਵਾਪਸ (FDT_FORCE fdt64_t)cpu_ਮੰਜ਼ਿਲ_be64(x);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __APPLE__
#ਅਨ੍ਤਰ੍ਭਾਵ <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# ਜੇ !defined(MAC_OS_X_ਸੰਸਕਰਣ_10_7) || (MAC_OS_X_ਸੰਸਕਰਣ_MAX_ALLOWED < \
                                         MAC_OS_X_ਸੰਸਕਰਣ_10_7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t fdt_strnlen(ਅਚਲ ਅੱਖਰ *string, size_t max_ਗਿਣਤੀ)
{
    ਅਚਲ ਅੱਖਰ *p = memchr(string, 0, max_ਗਿਣਤੀ);
    ਵਾਪਸ p ? p - string : max_ਗਿਣਤੀ;
}

#ਜੇ_ਅੰਤ /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#ਜੇ_ਅੰਤ /* __APPLE__ */

#ਜੇ_ਅੰਤ /* LIBFDT_ENV_H */
