/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_bitmap.h>

void __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
		  const unsigned long *bitmap2, 整数 bits)
{
	整数 k;
	整数 nr = BITS_TO_LONGS(bits);

	ために (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
		 const unsigned long *bitmap2, 整数 bits)
{
	整数 k;
	整数 nr = BITS_TO_LONGS(bits);

	ために (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
		  const unsigned long *bitmap2, 整数 bits)
{
	整数 k;
	整数 nr = BITS_TO_LONGS(bits);

	ために (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
