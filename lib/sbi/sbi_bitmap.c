/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitmap.h>

ଶୂନ୍ୟ __bitmap_and(unsigned long *dst, ଅଚଳ unsigned long *bitmap1,
		  ଅଚଳ unsigned long *bitmap2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr = BITS_TO_LONGS(bits);

	ପାଇଁ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

ଶୂନ୍ୟ __bitmap_or(unsigned long *dst, ଅଚଳ unsigned long *bitmap1,
		 ଅଚଳ unsigned long *bitmap2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr = BITS_TO_LONGS(bits);

	ପାଇଁ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

ଶୂନ୍ୟ __bitmap_xor(unsigned long *dst, ଅଚଳ unsigned long *bitmap1,
		  ଅଚଳ unsigned long *bitmap2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr = BITS_TO_LONGS(bits);

	ପାଇଁ (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
