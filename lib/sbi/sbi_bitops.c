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

#تعریف_کړه BITOP_کلمه(nr)		((nr) / بټونه_فی_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
بینښه اوږد پیدا_کړه_لومړی_بټ(ثابت بینښه اوږد *addr,
			     بینښه اوږد اندازه)
{
	ثابت بینښه اوږد *p = addr;
	بینښه اوږد پایله = 0;
	بینښه اوږد tmp;

	طالما (اندازه & ~(بټونه_فی_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب وموندل;
		پایله += بټونه_فی_LONG;
		اندازه -= بټونه_فی_LONG;
	}
	إذا (!اندازه)
		أرجع پایله;

	tmp = (*p) & (~0UL >> (بټونه_فی_LONG - اندازه));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع پایله + اندازه;	/* Nope. */
وموندل:
	أرجع پایله + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
بینښه اوږد پیدا_کړه_لومړی_zero_بټ(ثابت بینښه اوږد *addr,
				  بینښه اوږد اندازه)
{
	ثابت بینښه اوږد *p = addr;
	بینښه اوږد پایله = 0;
	بینښه اوږد tmp;

	طالما (اندازه & ~(بټونه_فی_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب وموندل;
		پایله += بټونه_فی_LONG;
		اندازه -= بټونه_فی_LONG;
	}
	إذا (!اندازه)
		أرجع پایله;

	tmp = (*p) | (~0UL << اندازه);
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع پایله + اندازه;	/* Nope. */
وموندل:
	أرجع پایله + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
بینښه اوږد پیدا_کړه_وروستی_بټ(ثابت بینښه اوږد *addr,
			    بینښه اوږد اندازه)
{
	بینښه اوږد ټکي;
	بینښه اوږد tmp;

	/* Start at final word. */
	ټکي = اندازه / بټونه_فی_LONG;

	/* Partial final word? */
	إذا (اندازه & (بټونه_فی_LONG-1)) {
		tmp = (addr[ټکي] & (~0UL >> (بټونه_فی_LONG
					 - (اندازه & (بټونه_فی_LONG-1)))));
		إذا (tmp)
			اذهب وموندل;
	}

	طالما (ټکي) {
		tmp = addr[--ټکي];
		إذا (tmp) {
وموندل:
			أرجع ټکي * بټونه_فی_LONG + sbi_fls(tmp);
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
بینښه اوږد پیدا_کړه_بل_بټ(ثابت بینښه اوږد *addr,
			    بینښه اوږد اندازه, بینښه اوږد دننه_واټن)
{
	ثابت بینښه اوږد *p = addr + BITOP_کلمه(دننه_واټن);
	بینښه اوږد پایله = دننه_واټن & ~(بټونه_فی_LONG-1);
	بینښه اوږد tmp;

	إذا (دننه_واټن >= اندازه)
		أرجع اندازه;
	اندازه -= پایله;
	دننه_واټن %= بټونه_فی_LONG;
	إذا (دننه_واټن) {
		tmp = *(p++);
		tmp &= (~0UL << دننه_واټن);
		إذا (اندازه < بټونه_فی_LONG)
			اذهب وموندل_لومړی;
		إذا (tmp)
			اذهب وموندل_منځ;
		اندازه -= بټونه_فی_LONG;
		پایله += بټونه_فی_LONG;
	}
	طالما (اندازه & ~(بټونه_فی_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب وموندل_منځ;
		پایله += بټونه_فی_LONG;
		اندازه -= بټونه_فی_LONG;
	}
	إذا (!اندازه)
		أرجع پایله;
	tmp = *p;

وموندل_لومړی:
	tmp &= (~0UL >> (بټونه_فی_LONG - اندازه));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع پایله + اندازه;	/* Nope. */
وموندل_منځ:
	أرجع پایله + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بینښه اوږد پیدا_کړه_بل_zero_بټ(ثابت بینښه اوږد *addr,
				 بینښه اوږد اندازه,
				 بینښه اوږد دننه_واټن)
{
	ثابت بینښه اوږد *p = addr + BITOP_کلمه(دننه_واټن);
	بینښه اوږد پایله = دننه_واټن & ~(بټونه_فی_LONG-1);
	بینښه اوږد tmp;

	إذا (دننه_واټن >= اندازه)
		أرجع اندازه;
	اندازه -= پایله;
	دننه_واټن %= بټونه_فی_LONG;
	إذا (دننه_واټن) {
		tmp = *(p++);
		tmp |= ~0UL >> (بټونه_فی_LONG - دننه_واټن);
		إذا (اندازه < بټونه_فی_LONG)
			اذهب وموندل_لومړی;
		إذا (~tmp)
			اذهب وموندل_منځ;
		اندازه -= بټونه_فی_LONG;
		پایله += بټونه_فی_LONG;
	}
	طالما (اندازه & ~(بټونه_فی_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب وموندل_منځ;
		پایله += بټونه_فی_LONG;
		اندازه -= بټونه_فی_LONG;
	}
	إذا (!اندازه)
		أرجع پایله;
	tmp = *p;

وموندل_لومړی:
	tmp |= ~0UL << اندازه;
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع پایله + اندازه;	/* Nope. */
وموندل_منځ:
	أرجع پایله + sbi_ffz(tmp);
}
