/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_bitmap.h>

تهی __bitmap_and(unsigned long *dst, ثابت unsigned long *bitmap1,
		  ثابت unsigned long *bitmap2, صحیح bits)
{
	صحیح k;
	صحیح nr = BITS_TO_LONGS(bits);

	برای (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

تهی __bitmap_or(unsigned long *dst, ثابت unsigned long *bitmap1,
		 ثابت unsigned long *bitmap2, صحیح bits)
{
	صحیح k;
	صحیح nr = BITS_TO_LONGS(bits);

	برای (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

تهی __bitmap_xor(unsigned long *dst, ثابت unsigned long *bitmap1,
		  ثابت unsigned long *bitmap2, صحیح bits)
{
	صحیح k;
	صحیح nr = BITS_TO_LONGS(bits);

	برای (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
