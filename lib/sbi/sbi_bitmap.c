/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitmap.h>

فراغ __bitmap_and(بنانشان ڊگهو *dst, ثابت بنانشان ڊگهو *bitmap1,
		  ثابت بنانشان ڊگهو *bitmap2, صحيح بٽ)
{
	صحيح k;
	صحيح nr = بٽ_منزل_LONGS(بٽ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

فراغ __bitmap_or(بنانشان ڊگهو *dst, ثابت بنانشان ڊگهو *bitmap1,
		 ثابت بنانشان ڊگهو *bitmap2, صحيح بٽ)
{
	صحيح k;
	صحيح nr = بٽ_منزل_LONGS(بٽ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

فراغ __bitmap_xor(بنانشان ڊگهو *dst, ثابت بنانشان ڊگهو *bitmap1,
		  ثابت بنانشان ڊگهو *bitmap2, صحيح بٽ)
{
	صحيح k;
	صحيح nr = بٽ_منزل_LONGS(بٽ);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
