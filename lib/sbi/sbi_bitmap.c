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
		  const unsigned long *bitmap2, số_nguyên bits)
{
	số_nguyên k;
	số_nguyên nr = BITS_TO_LONGS(bits);

	cho (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
		 const unsigned long *bitmap2, số_nguyên bits)
{
	số_nguyên k;
	số_nguyên nr = BITS_TO_LONGS(bits);

	cho (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
		  const unsigned long *bitmap2, số_nguyên bits)
{
	số_nguyên k;
	số_nguyên nr = BITS_TO_LONGS(bits);

	cho (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
