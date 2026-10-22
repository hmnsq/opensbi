/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#अगर_अघोषित LIBFDT_ENV_H
#घोषणा LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_byteorder.h>

#घोषणा पूर्णांक_उच्च		((पूर्णांक)(~0U >> 1))
#घोषणा अच_पूर्णांक_उच्च	((अचिन्हित पूर्णांक)~0U)
#घोषणा INT32_MAX	पूर्णांक_उच्च
#घोषणा UINT32_MAX	अच_पूर्णांक_उच्च

#अगर_घोषित __CHECKER__
#घोषणा FDT_FORCE __attribute__((force))
#घोषणा FDT_BITWISE __attribute__((bitwise))
#अन्यथा
#घोषणा FDT_FORCE
#घोषणा FDT_BITWISE
#पूर्ण_अगर

#घोषणा स_हटाओ		sbi_memmove
#घोषणा स_नकल		sbi_memcpy
#घोषणा स_भेद		sbi_memcmp
#घोषणा स_प्रथम		sbi_memchr
#घोषणा स_रखो		sbi_memset
#घोषणा म_अक्षर		sbi_strchr
#घोषणा म_खोजप		sbi_strrchr
#घोषणा म_नकल		sbi_strcpy
#घोषणा म_नकलन		sbi_strncpy
#घोषणा म_भेद		sbi_strcmp
#घोषणा म_भेदन		sbi_strncmp
#घोषणा म_माप		sbi_strlen
#घोषणा strnlen		sbi_strnlen

प्रकार be16_t FDT_BITWISE fdt16_t;
प्रकार be32_t FDT_BITWISE fdt32_t;
प्रकार be64_t FDT_BITWISE fdt64_t;

अटल अंतरभूत uint16_t fdt16_to_cpu(fdt16_t x)
{
	वापस (FDT_FORCE uint16_t)be16_to_cpu(x);
}

अटल अंतरभूत fdt16_t cpu_to_fdt16(uint16_t x)
{
	वापस (FDT_FORCE fdt16_t)cpu_to_be16(x);
}

अटल अंतरभूत uint32_t fdt32_to_cpu(fdt32_t x)
{
	वापस (FDT_FORCE uint32_t)be32_to_cpu(x);
}

अटल अंतरभूत fdt32_t cpu_to_fdt32(uint32_t x)
{
	वापस (FDT_FORCE fdt32_t)cpu_to_be32(x);
}

अटल अंतरभूत uint64_t fdt64_to_cpu(fdt64_t x)
{
	वापस (FDT_FORCE uint64_t)be64_to_cpu(x);
}

अटल अंतरभूत fdt64_t cpu_to_fdt64(uint64_t x)
{
	वापस (FDT_FORCE fdt64_t)cpu_to_be64(x);
}

#अगर_घोषित __APPLE__
#समावेश <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# अगर !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED < \
                                         MAC_OS_X_VERSION_10_7)

#घोषणा strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
अटल अंतरभूत माप_प्रकार fdt_strnlen(स्थिर अक्षर *string, माप_प्रकार max_गिनती)
{
    स्थिर अक्षर *p = स_प्रथम(string, 0, max_गिनती);
    वापस p ? p - string : max_गिनती;
}

#पूर्ण_अगर /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#पूर्ण_अगर /* __APPLE__ */

#पूर्ण_अगर /* LIBFDT_ENV_H */
