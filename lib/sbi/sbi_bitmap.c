/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>

ਖਾਲੀ __bitmap_and(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *dst, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap1,
		  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap2, ਪੂਰਨ ਬਿੱਟ)
{
	ਪੂਰਨ k;
	ਪੂਰਨ nr = ਬਿੱਟ_ਮੰਜ਼ਿਲ_LONGS(ਬਿੱਟ);

	ਲਈ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

ਖਾਲੀ __bitmap_or(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *dst, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap1,
		 ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap2, ਪੂਰਨ ਬਿੱਟ)
{
	ਪੂਰਨ k;
	ਪੂਰਨ nr = ਬਿੱਟ_ਮੰਜ਼ਿਲ_LONGS(ਬਿੱਟ);

	ਲਈ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

ਖਾਲੀ __bitmap_xor(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *dst, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap1,
		  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *bitmap2, ਪੂਰਨ ਬਿੱਟ)
{
	ਪੂਰਨ k;
	ਪੂਰਨ nr = ਬਿੱਟ_ਮੰਜ਼ਿਲ_LONGS(ਬਿੱਟ);

	ਲਈ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
