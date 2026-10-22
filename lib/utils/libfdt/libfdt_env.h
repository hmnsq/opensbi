/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LIBFDT_ENV_H
#ವ್ಯಾಖ್ಯಾನಿಸು LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_byteorder.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ		((ಪೂರ್ಣಾಂಕ)(~0U >> 1))
#ವ್ಯಾಖ್ಯಾನಿಸು UINT_MAX	((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)~0U)
#ವ್ಯಾಖ್ಯಾನಿಸು INT32_MAX	ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
#ವ್ಯಾಖ್ಯಾನಿಸು UINT32_MAX	UINT_MAX

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CHECKER__
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_FORCE __attribute__((force))
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_BITWISE __attribute__((bitwise))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_FORCE
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_BITWISE
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು memmove		sbi_memmove
#ವ್ಯಾಖ್ಯಾನಿಸು ಸ್ಮರಣೆ_ನಕಲು		sbi_memcpy
#ವ್ಯಾಖ್ಯಾನಿಸು memcmp		sbi_memcmp
#ವ್ಯಾಖ್ಯಾನಿಸು memchr		sbi_memchr
#ವ್ಯಾಖ್ಯಾನಿಸು memset		sbi_memset
#ವ್ಯಾಖ್ಯಾನಿಸು strchr		sbi_strchr
#ವ್ಯಾಖ್ಯಾನಿಸು strrchr		sbi_strrchr
#ವ್ಯಾಖ್ಯಾನಿಸು strcpy		sbi_strcpy
#ವ್ಯಾಖ್ಯಾನಿಸು strncpy		sbi_strncpy
#ವ್ಯಾಖ್ಯಾನಿಸು strcmp		sbi_strcmp
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ		sbi_strncmp
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಕ್ಷರಸರಣಿ_ಉದ್ದ		sbi_strlen
#ವ್ಯಾಖ್ಯಾನಿಸು strnlen		sbi_strnlen

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ be16_t FDT_BITWISE fdt16_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ be32_t FDT_BITWISE fdt32_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ be64_t FDT_BITWISE fdt64_t;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint16_t fdt16_ಗಮ್ಯ_cpu(fdt16_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE uint16_t)be16_ಗಮ್ಯ_cpu(x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ fdt16_t cpu_ಗಮ್ಯ_fdt16(uint16_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE fdt16_t)cpu_ಗಮ್ಯ_be16(x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t fdt32_ಗಮ್ಯ_cpu(fdt32_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE uint32_t)be32_ಗಮ್ಯ_cpu(x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ fdt32_t cpu_ಗಮ್ಯ_fdt32(uint32_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE fdt32_t)cpu_ಗಮ್ಯ_be32(x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t fdt64_ಗಮ್ಯ_cpu(fdt64_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE uint64_t)be64_ಗಮ್ಯ_cpu(x);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ fdt64_t cpu_ಗಮ್ಯ_fdt64(uint64_t x)
{
	ಹಿಂತಿರುಗಿ (FDT_FORCE fdt64_t)cpu_ಗಮ್ಯ_be64(x);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __APPLE__
#ಅನ್ತರ್ಭಾವ <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# ಆದರೆ !defined(MAC_OS_X_ಆವೃತ್ತಿ_10_7) || (MAC_OS_X_ಆವೃತ್ತಿ_MAX_ALLOWED < \
                                         MAC_OS_X_ಆವೃತ್ತಿ_10_7)

#ವ್ಯಾಖ್ಯಾನಿಸು strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t fdt_strnlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, size_t max_ಎಣಿಕೆ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = memchr(string, 0, max_ಎಣಿಕೆ);
    ಹಿಂತಿರುಗಿ p ? p - string : max_ಎಣಿಕೆ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#ಆದರೆ_ಅಂತ್ಯ /* __APPLE__ */

#ಆದರೆ_ಅಂತ್ಯ /* LIBFDT_ENV_H */
