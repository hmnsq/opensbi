/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitmap.h>

فراغ __bitmap_and(unsigned long *dst, ثابت unsigned long *bitmap1,
		  ثابت unsigned long *bitmap2, صحيح bits)
{
	صحيح k;
	صحيح nr = BITS_TO_LONGS(bits);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

فراغ __bitmap_or(unsigned long *dst, ثابت unsigned long *bitmap1,
		 ثابت unsigned long *bitmap2, صحيح bits)
{
	صحيح k;
	صحيح nr = BITS_TO_LONGS(bits);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

فراغ __bitmap_xor(unsigned long *dst, ثابت unsigned long *bitmap1,
		  ثابت unsigned long *bitmap2, صحيح bits)
{
	صحيح k;
	صحيح nr = BITS_TO_LONGS(bits);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
