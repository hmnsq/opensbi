/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>

ಶೂನ್ಯ __bitmap_and(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *dst, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap1,
		  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap2, ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು)
{
	ಪೂರ್ಣಾಂಕ k;
	ಪೂರ್ಣಾಂಕ nr = ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_LONGS(ಬಿಟ್ಟುಗಳು);

	ಗಾಗಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

ಶೂನ್ಯ __bitmap_or(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *dst, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap1,
		 ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap2, ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು)
{
	ಪೂರ್ಣಾಂಕ k;
	ಪೂರ್ಣಾಂಕ nr = ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_LONGS(ಬಿಟ್ಟುಗಳು);

	ಗಾಗಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

ಶೂನ್ಯ __bitmap_xor(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *dst, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap1,
		  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *bitmap2, ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು)
{
	ಪೂರ್ಣಾಂಕ k;
	ಪೂರ್ಣಾಂಕ nr = ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_LONGS(ಬಿಟ್ಟುಗಳು);

	ಗಾಗಿ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
