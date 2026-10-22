/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>

शून्य __bitmap_and(unsigned long *dst, ध्रुव unsigned long *bitmap1,
		  ध्रुव unsigned long *bitmap2, पूर्णाङ्क bits)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = BITS_TO_LONGS(bits);

	अत्थाय (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

शून्य __bitmap_or(unsigned long *dst, ध्रुव unsigned long *bitmap1,
		 ध्रुव unsigned long *bitmap2, पूर्णाङ्क bits)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = BITS_TO_LONGS(bits);

	अत्थाय (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

शून्य __bitmap_xor(unsigned long *dst, ध्रुव unsigned long *bitmap1,
		  ध्रुव unsigned long *bitmap2, पूर्णाङ्क bits)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = BITS_TO_LONGS(bits);

	अत्थाय (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
