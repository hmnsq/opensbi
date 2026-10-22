/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitmap.h>

فراغ __bitmap_and(بیعلامت دراز *dst, ثابت بیعلامت دراز *bitmap1,
		  ثابت بیعلامت دراز *bitmap2, صحيح بیتها)
{
	صحيح k;
	صحيح nr = بیتها_مقصد_LONGS(بیتها);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

فراغ __bitmap_or(بیعلامت دراز *dst, ثابت بیعلامت دراز *bitmap1,
		 ثابت بیعلامت دراز *bitmap2, صحيح بیتها)
{
	صحيح k;
	صحيح nr = بیتها_مقصد_LONGS(بیتها);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

فراغ __bitmap_xor(بیعلامت دراز *dst, ثابت بیعلامت دراز *bitmap1,
		  ثابت بیعلامت دراز *bitmap2, صحيح بیتها)
{
	صحيح k;
	صحيح nr = بیتها_مقصد_LONGS(بیتها);

	لكل (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
