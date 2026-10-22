/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#నిర్వచితం_కాకపోతే LIBFDT_ENV_H
#నిర్వచించు LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_byteorder.h>

#నిర్వచించు గరిష్ఠ_పూర్ణాంకం		((పూర్ణాంకం)(~0U >> 1))
#నిర్వచించు UINT_MAX	((గుర్తులేని పూర్ణాంకం)~0U)
#నిర్వచించు INT32_MAX	గరిష్ఠ_పూర్ణాంకం
#నిర్వచించు UINT32_MAX	UINT_MAX

#నిర్వచితమైతే __CHECKER__
#నిర్వచించు FDT_FORCE __attribute__((force))
#నిర్వచించు FDT_BITWISE __attribute__((bitwise))
#లేకపోతే
#నిర్వచించు FDT_FORCE
#నిర్వచించు FDT_BITWISE
#అయితే_ముగింపు

#నిర్వచించు memmove		sbi_memmove
#నిర్వచించు జ్ఞాపకం_నకలు		sbi_memcpy
#నిర్వచించు memcmp		sbi_memcmp
#నిర్వచించు memchr		sbi_memchr
#నిర్వచించు memset		sbi_memset
#నిర్వచించు strchr		sbi_strchr
#నిర్వచించు strrchr		sbi_strrchr
#నిర్వచించు strcpy		sbi_strcpy
#నిర్వచించు strncpy		sbi_strncpy
#నిర్వచించు strcmp		sbi_strcmp
#నిర్వచించు అక్షరశ్రేణి_పోలిక		sbi_strncmp
#నిర్వచించు అక్షరశ్రేణి_పొడవు		sbi_strlen
#నిర్వచించు strnlen		sbi_strnlen

రకనిర్వచనం be16_t FDT_BITWISE fdt16_t;
రకనిర్వచనం be32_t FDT_BITWISE fdt32_t;
రకనిర్వచనం be64_t FDT_BITWISE fdt64_t;

స్థిర అంతఃవరుస uint16_t fdt16_గమ్యం_cpu(fdt16_t x)
{
	తిరిగి (FDT_FORCE uint16_t)be16_గమ్యం_cpu(x);
}

స్థిర అంతఃవరుస fdt16_t cpu_గమ్యం_fdt16(uint16_t x)
{
	తిరిగి (FDT_FORCE fdt16_t)cpu_గమ్యం_be16(x);
}

స్థిర అంతఃవరుస uint32_t fdt32_గమ్యం_cpu(fdt32_t x)
{
	తిరిగి (FDT_FORCE uint32_t)be32_గమ్యం_cpu(x);
}

స్థిర అంతఃవరుస fdt32_t cpu_గమ్యం_fdt32(uint32_t x)
{
	తిరిగి (FDT_FORCE fdt32_t)cpu_గమ్యం_be32(x);
}

స్థిర అంతఃవరుస uint64_t fdt64_గమ్యం_cpu(fdt64_t x)
{
	తిరిగి (FDT_FORCE uint64_t)be64_గమ్యం_cpu(x);
}

స్థిర అంతఃవరుస fdt64_t cpu_గమ్యం_fdt64(uint64_t x)
{
	తిరిగి (FDT_FORCE fdt64_t)cpu_గమ్యం_be64(x);
}

#నిర్వచితమైతే __APPLE__
#అన్తర్భావ <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# ఒకవేళ !defined(MAC_OS_X_సంస్కరణ_10_7) || (MAC_OS_X_సంస్కరణ_MAX_ALLOWED < \
                                         MAC_OS_X_సంస్కరణ_10_7)

#నిర్వచించు strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
స్థిర అంతఃవరుస size_t fdt_strnlen(స్థిరరాశి అక్షరం *string, size_t max_లెక్క)
{
    స్థిరరాశి అక్షరం *p = memchr(string, 0, max_లెక్క);
    తిరిగి p ? p - string : max_లెక్క;
}

#అయితే_ముగింపు /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#అయితే_ముగింపు /* __APPLE__ */

#అయితే_ముగింపు /* LIBFDT_ENV_H */
