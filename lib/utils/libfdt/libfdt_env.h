/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#यदि_अपरिभाषितम् LIBFDT_ENV_H
#परिभाषय LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>

#परिभाषय अधिकतम_पूर्णाङ्कः		((पूर्णाङ्क)(~0U >> 1))
#परिभाषय UINT_MAX	((अचिह्नित पूर्णाङ्क)~0U)
#परिभाषय INT32_MAX	अधिकतम_पूर्णाङ्कः
#परिभाषय UINT32_MAX	UINT_MAX

#यदि_परिभाषितम् __CHECKER__
#परिभाषय FDT_FORCE __attribute__((force))
#परिभाषय FDT_BITWISE __attribute__((bitwise))
#अन्यथा
#परिभाषय FDT_FORCE
#परिभाषय FDT_BITWISE
#यदि_अन्तः

#परिभाषय memmove		sbi_memmove
#परिभाषय स्मृतिं_प्रतिलिख		sbi_memcpy
#परिभाषय memcmp		sbi_memcmp
#परिभाषय memchr		sbi_memchr
#परिभाषय स्मृतिं_पूरय		sbi_memset
#परिभाषय strchr		sbi_strchr
#परिभाषय strrchr		sbi_strrchr
#परिभाषय वाक्यं_प्रतिलिख		sbi_strcpy
#परिभाषय strncpy		sbi_strncpy
#परिभाषय वाक्यं_तुलय		sbi_strcmp
#परिभाषय वाक्ये_तुलय		sbi_strncmp
#परिभाषय वाक्यदैर्घ्यम्		sbi_strlen
#परिभाषय strnlen		sbi_strnlen

प्रकारनिर्देश be16_t FDT_BITWISE fdt16_t;
प्रकारनिर्देश be32_t FDT_BITWISE fdt32_t;
प्रकारनिर्देश be64_t FDT_BITWISE fdt64_t;

स्थिर अन्तःस्थ uint16_t fdt16_गन्तव्यम्_cpu(fdt16_t x)
{
	प्रत्यागम (FDT_FORCE uint16_t)be16_गन्तव्यम्_cpu(x);
}

स्थिर अन्तःस्थ fdt16_t cpu_गन्तव्यम्_fdt16(uint16_t x)
{
	प्रत्यागम (FDT_FORCE fdt16_t)cpu_गन्तव्यम्_be16(x);
}

स्थिर अन्तःस्थ uint32_t fdt32_गन्तव्यम्_cpu(fdt32_t x)
{
	प्रत्यागम (FDT_FORCE uint32_t)be32_गन्तव्यम्_cpu(x);
}

स्थिर अन्तःस्थ fdt32_t cpu_गन्तव्यम्_fdt32(uint32_t x)
{
	प्रत्यागम (FDT_FORCE fdt32_t)cpu_गन्तव्यम्_be32(x);
}

स्थिर अन्तःस्थ uint64_t fdt64_गन्तव्यम्_cpu(fdt64_t x)
{
	प्रत्यागम (FDT_FORCE uint64_t)be64_गन्तव्यम्_cpu(x);
}

स्थिर अन्तःस्थ fdt64_t cpu_गन्तव्यम्_fdt64(uint64_t x)
{
	प्रत्यागम (FDT_FORCE fdt64_t)cpu_गन्तव्यम्_be64(x);
}

#यदि_परिभाषितम् __APPLE__
#अन्तर्भाव <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# यदि !defined(MAC_OS_X_संस्करणम्_10_7) || (MAC_OS_X_संस्करणम्_MAX_ALLOWED < \
                                         MAC_OS_X_संस्करणम्_10_7)

#परिभाषय strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
स्थिर अन्तःस्थ size_t fdt_strnlen(ध्रुव वर्ण *string, size_t max_गणनम्)
{
    ध्रुव वर्ण *p = memchr(string, 0, max_गणनम्);
    प्रत्यागम p ? p - string : max_गणनम्;
}

#यदि_अन्तः /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#यदि_अन्तः /* __APPLE__ */

#यदि_अन्तः /* LIBFDT_ENV_H */
