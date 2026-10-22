/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_bitops.h>

#تعریف_کن BITOP_کلمه(nr)		((nr) / بیتها_در_هر_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
بیعلامت دراز پیدا_کن_اولی_بیت(ثابت بیعلامت دراز *addr,
			     بیعلامت دراز اندازه)
{
	ثابت بیعلامت دراز *p = addr;
	بیعلامت دراز نتیجه = 0;
	بیعلامت دراز tmp;

	طالما (اندازه & ~(بیتها_در_هر_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب یافت;
		نتیجه += بیتها_در_هر_LONG;
		اندازه -= بیتها_در_هر_LONG;
	}
	إذا (!اندازه)
		أرجع نتیجه;

	tmp = (*p) & (~0UL >> (بیتها_در_هر_LONG - اندازه));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتیجه + اندازه;	/* Nope. */
یافت:
	أرجع نتیجه + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
بیعلامت دراز پیدا_کن_اولی_zero_بیت(ثابت بیعلامت دراز *addr,
				  بیعلامت دراز اندازه)
{
	ثابت بیعلامت دراز *p = addr;
	بیعلامت دراز نتیجه = 0;
	بیعلامت دراز tmp;

	طالما (اندازه & ~(بیتها_در_هر_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب یافت;
		نتیجه += بیتها_در_هر_LONG;
		اندازه -= بیتها_در_هر_LONG;
	}
	إذا (!اندازه)
		أرجع نتیجه;

	tmp = (*p) | (~0UL << اندازه);
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتیجه + اندازه;	/* Nope. */
یافت:
	أرجع نتیجه + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
بیعلامت دراز پیدا_کن_آخرین_بیت(ثابت بیعلامت دراز *addr,
			    بیعلامت دراز اندازه)
{
	بیعلامت دراز واژگان;
	بیعلامت دراز tmp;

	/* Start at final word. */
	واژگان = اندازه / بیتها_در_هر_LONG;

	/* Partial final word? */
	إذا (اندازه & (بیتها_در_هر_LONG-1)) {
		tmp = (addr[واژگان] & (~0UL >> (بیتها_در_هر_LONG
					 - (اندازه & (بیتها_در_هر_LONG-1)))));
		إذا (tmp)
			اذهب یافت;
	}

	طالما (واژگان) {
		tmp = addr[--واژگان];
		إذا (tmp) {
یافت:
			أرجع واژگان * بیتها_در_هر_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	أرجع اندازه;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بیعلامت دراز پیدا_کن_بعدی_بیت(ثابت بیعلامت دراز *addr,
			    بیعلامت دراز اندازه, بیعلامت دراز جابه_جایی)
{
	ثابت بیعلامت دراز *p = addr + BITOP_کلمه(جابه_جایی);
	بیعلامت دراز نتیجه = جابه_جایی & ~(بیتها_در_هر_LONG-1);
	بیعلامت دراز tmp;

	إذا (جابه_جایی >= اندازه)
		أرجع اندازه;
	اندازه -= نتیجه;
	جابه_جایی %= بیتها_در_هر_LONG;
	إذا (جابه_جایی) {
		tmp = *(p++);
		tmp &= (~0UL << جابه_جایی);
		إذا (اندازه < بیتها_در_هر_LONG)
			اذهب یافت_اولی;
		إذا (tmp)
			اذهب یافت_میانه;
		اندازه -= بیتها_در_هر_LONG;
		نتیجه += بیتها_در_هر_LONG;
	}
	طالما (اندازه & ~(بیتها_در_هر_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب یافت_میانه;
		نتیجه += بیتها_در_هر_LONG;
		اندازه -= بیتها_در_هر_LONG;
	}
	إذا (!اندازه)
		أرجع نتیجه;
	tmp = *p;

یافت_اولی:
	tmp &= (~0UL >> (بیتها_در_هر_LONG - اندازه));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتیجه + اندازه;	/* Nope. */
یافت_میانه:
	أرجع نتیجه + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بیعلامت دراز پیدا_کن_بعدی_zero_بیت(ثابت بیعلامت دراز *addr,
				 بیعلامت دراز اندازه,
				 بیعلامت دراز جابه_جایی)
{
	ثابت بیعلامت دراز *p = addr + BITOP_کلمه(جابه_جایی);
	بیعلامت دراز نتیجه = جابه_جایی & ~(بیتها_در_هر_LONG-1);
	بیعلامت دراز tmp;

	إذا (جابه_جایی >= اندازه)
		أرجع اندازه;
	اندازه -= نتیجه;
	جابه_جایی %= بیتها_در_هر_LONG;
	إذا (جابه_جایی) {
		tmp = *(p++);
		tmp |= ~0UL >> (بیتها_در_هر_LONG - جابه_جایی);
		إذا (اندازه < بیتها_در_هر_LONG)
			اذهب یافت_اولی;
		إذا (~tmp)
			اذهب یافت_میانه;
		اندازه -= بیتها_در_هر_LONG;
		نتیجه += بیتها_در_هر_LONG;
	}
	طالما (اندازه & ~(بیتها_در_هر_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب یافت_میانه;
		نتیجه += بیتها_در_هر_LONG;
		اندازه -= بیتها_در_هر_LONG;
	}
	إذا (!اندازه)
		أرجع نتیجه;
	tmp = *p;

یافت_اولی:
	tmp |= ~0UL << اندازه;
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتیجه + اندازه;	/* Nope. */
یافت_میانه:
	أرجع نتیجه + sbi_ffz(tmp);
}
