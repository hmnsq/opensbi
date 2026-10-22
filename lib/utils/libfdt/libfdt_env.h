/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ifndef LIBFDT_ENV_H
#define LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_byteorder.h>

#define INT_MAX		((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(~0U >> 1))
#define UINT_MAX	((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)~0U)
#define INT32_MAX	INT_MAX
#define UINT32_MAX	UINT_MAX

#ifdef __CHECKER__
#define FDT_FORCE __attribute__((force))
#define FDT_BITWISE __attribute__((bitwise))
#ଅନ୍ୟଥା
#define FDT_FORCE
#define FDT_BITWISE
#endif

#define memmove		sbi_memmove
#define memcpy		sbi_memcpy
#define memcmp		sbi_memcmp
#define memchr		sbi_memchr
#define memset		sbi_memset
#define strchr		sbi_strchr
#define strrchr		sbi_strrchr
#define strcpy		sbi_strcpy
#define strncpy		sbi_strncpy
#define strcmp		sbi_strcmp
#define strncmp		sbi_strncmp
#define strlen		sbi_strlen
#define strnlen		sbi_strnlen

ପ୍ରକାରବ୍ୟାଖ୍ୟା be16_t FDT_BITWISE fdt16_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା be32_t FDT_BITWISE fdt32_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା be64_t FDT_BITWISE fdt64_t;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint16_t fdt16_to_cpu(fdt16_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE uint16_t)be16_to_cpu(x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ fdt16_t cpu_to_fdt16(uint16_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE fdt16_t)cpu_to_be16(x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t fdt32_to_cpu(fdt32_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE uint32_t)be32_to_cpu(x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ fdt32_t cpu_to_fdt32(uint32_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE fdt32_t)cpu_to_be32(x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t fdt64_to_cpu(fdt64_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE uint64_t)be64_to_cpu(x);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ fdt64_t cpu_to_fdt64(uint64_t x)
{
	ଫେରନ୍ତୁ (FDT_FORCE fdt64_t)cpu_to_be64(x);
}

#ifdef __APPLE__
#ଅନ୍ତର୍ଭାଵ <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# ଯଦି !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED < \
                                         MAC_OS_X_VERSION_10_7)

#define strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t fdt_strnlen(ଅଚଳ ଅକ୍ଷର *string, size_t max_ଗଣନା)
{
    ଅଚଳ ଅକ୍ଷର *p = memchr(string, 0, max_ଗଣନା);
    ଫେରନ୍ତୁ p ? p - string : max_ଗଣନା;
}

#endif /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#endif /* __APPLE__ */

#endif /* LIBFDT_ENV_H */
