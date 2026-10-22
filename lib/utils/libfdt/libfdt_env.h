/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ifndef LIBFDT_ENV_H
#define LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>

#define पूर्णांक_उच्च		((पूर्णाङ्क)(~0U >> 1))
#define अच_पूर्णांक_उच्च	((अचिन्हित पूर्णाङ्क)~0U)
#define INT32_MAX	पूर्णांक_उच्च
#define UINT32_MAX	अच_पूर्णांक_उच्च

#ifdef __CHECKER__
#define FDT_FORCE __attribute__((force))
#define FDT_BITWISE __attribute__((bitwise))
#अन्यथा
#define FDT_FORCE
#define FDT_BITWISE
#endif

#define स_हटाओ		sbi_memmove
#define स_नकल		sbi_memcpy
#define स_भेद		sbi_memcmp
#define स_प्रथम		sbi_memchr
#define स_रखो		sbi_memset
#define म_अक्षर		sbi_strchr
#define म_खोजप		sbi_strrchr
#define म_नकल		sbi_strcpy
#define म_नकलन		sbi_strncpy
#define म_भेद		sbi_strcmp
#define म_भेदन		sbi_strncmp
#define म_माप		sbi_strlen
#define strnlen		sbi_strnlen

प्रकारनिर्देश be16_t FDT_BITWISE fdt16_t;
प्रकारनिर्देश be32_t FDT_BITWISE fdt32_t;
प्रकारनिर्देश be64_t FDT_BITWISE fdt64_t;

स्थिर अन्तःस्थ uint16_t fdt16_to_cpu(fdt16_t x)
{
	प्रत्यागम (FDT_FORCE uint16_t)be16_to_cpu(x);
}

स्थिर अन्तःस्थ fdt16_t cpu_to_fdt16(uint16_t x)
{
	प्रत्यागम (FDT_FORCE fdt16_t)cpu_to_be16(x);
}

स्थिर अन्तःस्थ uint32_t fdt32_to_cpu(fdt32_t x)
{
	प्रत्यागम (FDT_FORCE uint32_t)be32_to_cpu(x);
}

स्थिर अन्तःस्थ fdt32_t cpu_to_fdt32(uint32_t x)
{
	प्रत्यागम (FDT_FORCE fdt32_t)cpu_to_be32(x);
}

स्थिर अन्तःस्थ uint64_t fdt64_to_cpu(fdt64_t x)
{
	प्रत्यागम (FDT_FORCE uint64_t)be64_to_cpu(x);
}

स्थिर अन्तःस्थ fdt64_t cpu_to_fdt64(uint64_t x)
{
	प्रत्यागम (FDT_FORCE fdt64_t)cpu_to_be64(x);
}

#ifdef __APPLE__
#अन्तर्भाव <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# यदि !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED < \
                                         MAC_OS_X_VERSION_10_7)

#define strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
स्थिर अन्तःस्थ माप_प्रकार fdt_strnlen(ध्रुव वर्ण *string, माप_प्रकार max_गिनती)
{
    ध्रुव वर्ण *p = स_प्रथम(string, 0, max_गिनती);
    प्रत्यागम p ? p - string : max_गिनती;
}

#endif /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#endif /* __APPLE__ */

#endif /* LIBFDT_ENV_H */
