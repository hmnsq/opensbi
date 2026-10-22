/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#اگر_غیر_تعریف_شدہ LIBFDT_ENV_H
#تعریف_کرو LIBFDT_ENV_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * Copyright 2012 Kim Phillips, Freescale Semiconductor.
 */

#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_byteorder.h>

#تعریف_کرو زیادہ_سے_زیادہ_عدد		((صحیح)(~0U >> 1))
#تعریف_کرو UINT_MAX	((غیرعلامتی صحیح)~0U)
#تعریف_کرو INT32_MAX	زیادہ_سے_زیادہ_عدد
#تعریف_کرو UINT32_MAX	UINT_MAX

#اگر_تعریف_شدہ __CHECKER__
#تعریف_کرو FDT_FORCE __attribute__((force))
#تعریف_کرو FDT_BITWISE __attribute__((bitwise))
#ورنہ
#تعریف_کرو FDT_FORCE
#تعریف_کرو FDT_BITWISE
#اگر_ختم

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
#تعریف_کرو لڑی_موازنہ		sbi_strncmp
#تعریف_کرو لڑی_لمبائی		sbi_strlen
#تعریف_کرو strnlen		sbi_strnlen

قسمتعریف be16_t FDT_BITWISE fdt16_t;
قسمتعریف be32_t FDT_BITWISE fdt32_t;
قسمتعریف be64_t FDT_BITWISE fdt64_t;

ساکن اندرونی uint16_t fdt16_مقصد_cpu(fdt16_t x)
{
	واپس (FDT_FORCE uint16_t)be16_مقصد_cpu(x);
}

ساکن اندرونی fdt16_t cpu_مقصد_fdt16(uint16_t x)
{
	واپس (FDT_FORCE fdt16_t)cpu_مقصد_be16(x);
}

ساکن اندرونی uint32_t fdt32_مقصد_cpu(fdt32_t x)
{
	واپس (FDT_FORCE uint32_t)be32_مقصد_cpu(x);
}

ساکن اندرونی fdt32_t cpu_مقصد_fdt32(uint32_t x)
{
	واپس (FDT_FORCE fdt32_t)cpu_مقصد_be32(x);
}

ساکن اندرونی uint64_t fdt64_مقصد_cpu(fdt64_t x)
{
	واپس (FDT_FORCE uint64_t)be64_مقصد_cpu(x);
}

ساکن اندرونی fdt64_t cpu_مقصد_fdt64(uint64_t x)
{
	واپس (FDT_FORCE fdt64_t)cpu_مقصد_be64(x);
}

#اگر_تعریف_شدہ __APPLE__
#شامل <AvailabilityMacros.h>

/* strnlen() is not available on Mac OS < 10.7 */
# اگر !defined(MAC_OS_X_نسخہ_10_7) || (MAC_OS_X_نسخہ_MAX_ALLOWED < \
                                         MAC_OS_X_نسخہ_10_7)

#تعریف_کرو strnlen fdt_strnlen

/*
 * fdt_strnlen: returns the length of a string or max_count - which ever is
 * smallest.
 * Input 1 string: the string whose size is to be determined
 * Input 2 max_count: the maximum value returned by this function
 * Output: length of the string or max_count (the smallest of the two)
 */
ساکن اندرونی size_t fdt_strnlen(ثابت حرف *string, size_t max_گنتی)
{
    ثابت حرف *p = memchr(string, 0, max_گنتی);
    واپس p ? p - string : max_گنتی;
}

#اگر_ختم /* !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED <
          MAC_OS_X_VERSION_10_7) */

#اگر_ختم /* __APPLE__ */

#اگر_ختم /* LIBFDT_ENV_H */
