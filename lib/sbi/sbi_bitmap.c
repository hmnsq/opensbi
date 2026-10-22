/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>

शून्य __bitmap_and(अचिह्नित दीर्घ *dst, ध्रुव अचिह्नित दीर्घ *bitmap1,
		  ध्रुव अचिह्नित दीर्घ *bitmap2, पूर्णाङ्क द्वयङ्काः)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = द्वयङ्काः_गन्तव्यम्_LONGS(द्वयङ्काः);

	प्रति (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

शून्य __bitmap_or(अचिह्नित दीर्घ *dst, ध्रुव अचिह्नित दीर्घ *bitmap1,
		 ध्रुव अचिह्नित दीर्घ *bitmap2, पूर्णाङ्क द्वयङ्काः)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = द्वयङ्काः_गन्तव्यम्_LONGS(द्वयङ्काः);

	प्रति (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

शून्य __bitmap_xor(अचिह्नित दीर्घ *dst, ध्रुव अचिह्नित दीर्घ *bitmap1,
		  ध्रुव अचिह्नित दीर्घ *bitmap2, पूर्णाङ्क द्वयङ्काः)
{
	पूर्णाङ्क k;
	पूर्णाङ्क nr = द्वयङ्काः_गन्तव्यम्_LONGS(द्वयङ्काः);

	प्रति (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
