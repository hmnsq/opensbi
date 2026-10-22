/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_bitmap.h>

શૂન્ય __bitmap_and(unsigned long *dst, અચલ unsigned long *bitmap1,
		  અચલ unsigned long *bitmap2, પૂર્ણાંક bits)
{
	પૂર્ણાંક k;
	પૂર્ણાંક nr = BITS_TO_LONGS(bits);

	માટે (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

શૂન્ય __bitmap_or(unsigned long *dst, અચલ unsigned long *bitmap1,
		 અચલ unsigned long *bitmap2, પૂર્ણાંક bits)
{
	પૂર્ણાંક k;
	પૂર્ણાંક nr = BITS_TO_LONGS(bits);

	માટે (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

શૂન્ય __bitmap_xor(unsigned long *dst, અચલ unsigned long *bitmap1,
		  અચલ unsigned long *bitmap2, પૂર્ણાંક bits)
{
	પૂર્ણાંક k;
	પૂર્ણાંક nr = BITS_TO_LONGS(bits);

	માટે (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
