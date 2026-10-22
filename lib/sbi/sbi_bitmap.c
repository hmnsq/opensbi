/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_bitmap.h>

خالی __bitmap_and(غیرعلامتی طویل *dst, ثابت غیرعلامتی طویل *bitmap1,
		  ثابت غیرعلامتی طویل *bitmap2, صحیح بٹ)
{
	صحیح k;
	صحیح nr = بٹ_مقصد_LONGS(بٹ);

	برائے (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

خالی __bitmap_or(غیرعلامتی طویل *dst, ثابت غیرعلامتی طویل *bitmap1,
		 ثابت غیرعلامتی طویل *bitmap2, صحیح بٹ)
{
	صحیح k;
	صحیح nr = بٹ_مقصد_LONGS(بٹ);

	برائے (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

خالی __bitmap_xor(غیرعلامتی طویل *dst, ثابت غیرعلامتی طویل *bitmap1,
		  ثابت غیرعلامتی طویل *bitmap2, صحیح بٹ)
{
	صحیح k;
	صحیح nr = بٹ_مقصد_LONGS(بٹ);

	برائے (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
