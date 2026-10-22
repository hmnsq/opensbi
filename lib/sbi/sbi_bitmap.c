/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>

ಶೂನ್ಯ __bitmap_and(unsigned long *dst, ಧ್ರುವ unsigned long *bitmap1,
		  ಧ್ರುವ unsigned long *bitmap2, ಪೂರ್ಣಾಙ್ಕ bits)
{
	ಪೂರ್ಣಾಙ್ಕ k;
	ಪೂರ್ಣಾಙ್ಕ nr = BITS_TO_LONGS(bits);

	ಪ್ರತಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

ಶೂನ್ಯ __bitmap_or(unsigned long *dst, ಧ್ರುವ unsigned long *bitmap1,
		 ಧ್ರುವ unsigned long *bitmap2, ಪೂರ್ಣಾಙ್ಕ bits)
{
	ಪೂರ್ಣಾಙ್ಕ k;
	ಪೂರ್ಣಾಙ್ಕ nr = BITS_TO_LONGS(bits);

	ಪ್ರತಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

ಶೂನ್ಯ __bitmap_xor(unsigned long *dst, ಧ್ರುವ unsigned long *bitmap1,
		  ಧ್ರುವ unsigned long *bitmap2, ಪೂರ್ಣಾಙ್ಕ bits)
{
	ಪೂರ್ಣಾಙ್ಕ k;
	ಪೂರ್ಣಾಙ್ಕ nr = BITS_TO_LONGS(bits);

	ಪ್ರತಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
