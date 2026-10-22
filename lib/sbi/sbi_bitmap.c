/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_bitmap.h>

வெறுமை __bitmap_and(unsigned long *dst, மாறிலி unsigned long *bitmap1,
		  மாறிலி unsigned long *bitmap2, முழு bits)
{
	முழு k;
	முழு nr = BITS_TO_LONGS(bits);

	க்கு (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

வெறுமை __bitmap_or(unsigned long *dst, மாறிலி unsigned long *bitmap1,
		 மாறிலி unsigned long *bitmap2, முழு bits)
{
	முழு k;
	முழு nr = BITS_TO_LONGS(bits);

	க்கு (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

வெறுமை __bitmap_xor(unsigned long *dst, மாறிலி unsigned long *bitmap1,
		  மாறிலி unsigned long *bitmap2, முழு bits)
{
	முழு k;
	முழு nr = BITS_TO_LONGS(bits);

	க்கு (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
