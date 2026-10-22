/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_bitmap.h>

శూన్య __bitmap_and(గుర్తులేని దీర్ఘ *dst, స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap1,
		  స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap2, పూర్ణాంకం బిట్లు)
{
	పూర్ణాంకం k;
	పూర్ణాంకం nr = బిట్లు_గమ్యం_LONGS(బిట్లు);

	కొరకు (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

శూన్య __bitmap_or(గుర్తులేని దీర్ఘ *dst, స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap1,
		 స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap2, పూర్ణాంకం బిట్లు)
{
	పూర్ణాంకం k;
	పూర్ణాంకం nr = బిట్లు_గమ్యం_LONGS(బిట్లు);

	కొరకు (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

శూన్య __bitmap_xor(గుర్తులేని దీర్ఘ *dst, స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap1,
		  స్థిరరాశి గుర్తులేని దీర్ఘ *bitmap2, పూర్ణాంకం బిట్లు)
{
	పూర్ణాంకం k;
	పూర్ణాంకం nr = బిట్లు_గమ్యం_LONGS(బిట్లు);

	కొరకు (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
