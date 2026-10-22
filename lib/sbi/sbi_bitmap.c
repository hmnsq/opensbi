/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_bitmap.h>

ശൂന്യം __bitmap_and(unsigned long *dst, സ്ഥിരം unsigned long *bitmap1,
		  സ്ഥിരം unsigned long *bitmap2, പൂർണ്ണസംഖ്യ bits)
{
	പൂർണ്ണസംഖ്യ k;
	പൂർണ്ണസംഖ്യ nr = BITS_TO_LONGS(bits);

	വേണ്ടി (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] & bitmap2[k];
}

ശൂന്യം __bitmap_or(unsigned long *dst, സ്ഥിരം unsigned long *bitmap1,
		 സ്ഥിരം unsigned long *bitmap2, പൂർണ്ണസംഖ്യ bits)
{
	പൂർണ്ണസംഖ്യ k;
	പൂർണ്ണസംഖ്യ nr = BITS_TO_LONGS(bits);

	വേണ്ടി (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}

ശൂന്യം __bitmap_xor(unsigned long *dst, സ്ഥിരം unsigned long *bitmap1,
		  സ്ഥിരം unsigned long *bitmap2, പൂർണ്ണസംഖ്യ bits)
{
	പൂർണ്ണസംഖ്യ k;
	പൂർണ്ണസംഖ്യ nr = BITS_TO_LONGS(bits);

	വേണ്ടി (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
