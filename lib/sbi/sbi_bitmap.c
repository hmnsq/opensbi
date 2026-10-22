/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitmap.h>

فراغ __bitmap_and(بینښه اوږد *dst, ثابت بینښه اوږد *bitmap1,
		  ثابت بینښه اوږد *bitmap2, صحيح بټونه)
{
	صحيح k;
	صحيح nr = بټونه_منزل_LONGS(بټونه);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

فراغ __bitmap_or(بینښه اوږد *dst, ثابت بینښه اوږد *bitmap1,
		 ثابت بینښه اوږد *bitmap2, صحيح بټونه)
{
	صحيح k;
	صحيح nr = بټونه_منزل_LONGS(بټونه);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

فراغ __bitmap_xor(بینښه اوږد *dst, ثابت بینښه اوږد *bitmap1,
		  ثابت بینښه اوږد *bitmap2, صحيح بټونه)
{
	صحيح k;
	صحيح nr = بټونه_منزل_LONGS(بټونه);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
