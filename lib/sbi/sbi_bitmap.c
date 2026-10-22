/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_bitmap.h>

শূন্য __bitmap_and(unsigned long *dst, ধ্রুব unsigned long *bitmap1,
		  ধ্রুব unsigned long *bitmap2, পূর্ণাঙ্ক bits)
{
	পূর্ণাঙ্ক k;
	পূর্ণাঙ্ক nr = BITS_TO_LONGS(bits);

	প্রতি (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

শূন্য __bitmap_or(unsigned long *dst, ধ্রুব unsigned long *bitmap1,
		 ধ্রুব unsigned long *bitmap2, পূর্ণাঙ্ক bits)
{
	পূর্ণাঙ্ক k;
	পূর্ণাঙ্ক nr = BITS_TO_LONGS(bits);

	প্রতি (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

শূন্য __bitmap_xor(unsigned long *dst, ধ্রুব unsigned long *bitmap1,
		  ধ্রুব unsigned long *bitmap2, পূর্ণাঙ্ক bits)
{
	পূর্ণাঙ্ক k;
	পূর্ণাঙ্ক nr = BITS_TO_LONGS(bits);

	প্রতি (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
