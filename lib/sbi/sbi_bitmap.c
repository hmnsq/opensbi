/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitmap.h>

فراغ __bitmap_and(بنانشانی لما *dst, ثابت بنانشانی لما *bitmap1,
		  ثابت بنانشانی لما *bitmap2, صحيح بٹ)
{
	صحيح k;
	صحيح nr = بٹ_منزل_LONGS(بٹ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

فراغ __bitmap_or(بنانشانی لما *dst, ثابت بنانشانی لما *bitmap1,
		 ثابت بنانشانی لما *bitmap2, صحيح بٹ)
{
	صحيح k;
	صحيح nr = بٹ_منزل_LONGS(بٹ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

فراغ __bitmap_xor(بنانشانی لما *dst, ثابت بنانشانی لما *bitmap1,
		  ثابت بنانشانی لما *bitmap2, صحيح بٹ)
{
	صحيح k;
	صحيح nr = بٹ_منزل_LONGS(بٹ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
