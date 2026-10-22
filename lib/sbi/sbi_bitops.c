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

#وصف_ڪريو BITOP_لفظ(nr)		((nr) / بٽ_في_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
بنانشان ڊگهو ڳوليو_پهريون_ٻٽو_انگ(ثابت بنانشان ڊگهو *addr,
			     بنانشان ڊگهو سائيز)
{
	ثابت بنانشان ڊگهو *p = addr;
	بنانشان ڊگهو نتيجو = 0;
	بنانشان ڊگهو tmp;

	طالما (سائيز & ~(بٽ_في_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب مليو;
		نتيجو += بٽ_في_LONG;
		سائيز -= بٽ_في_LONG;
	}
	إذا (!سائيز)
		أرجع نتيجو;

	tmp = (*p) & (~0UL >> (بٽ_في_LONG - سائيز));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتيجو + سائيز;	/* Nope. */
مليو:
	أرجع نتيجو + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
بنانشان ڊگهو ڳوليو_پهريون_zero_ٻٽو_انگ(ثابت بنانشان ڊگهو *addr,
				  بنانشان ڊگهو سائيز)
{
	ثابت بنانشان ڊگهو *p = addr;
	بنانشان ڊگهو نتيجو = 0;
	بنانشان ڊگهو tmp;

	طالما (سائيز & ~(بٽ_في_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب مليو;
		نتيجو += بٽ_في_LONG;
		سائيز -= بٽ_في_LONG;
	}
	إذا (!سائيز)
		أرجع نتيجو;

	tmp = (*p) | (~0UL << سائيز);
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتيجو + سائيز;	/* Nope. */
مليو:
	أرجع نتيجو + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
بنانشان ڊگهو ڳوليو_آخري_ٻٽو_انگ(ثابت بنانشان ڊگهو *addr,
			    بنانشان ڊگهو سائيز)
{
	بنانشان ڊگهو لفظ;
	بنانشان ڊگهو tmp;

	/* Start at final word. */
	لفظ = سائيز / بٽ_في_LONG;

	/* Partial final word? */
	إذا (سائيز & (بٽ_في_LONG-1)) {
		tmp = (addr[لفظ] & (~0UL >> (بٽ_في_LONG
					 - (سائيز & (بٽ_في_LONG-1)))));
		إذا (tmp)
			اذهب مليو;
	}

	طالما (لفظ) {
		tmp = addr[--لفظ];
		إذا (tmp) {
مليو:
			أرجع لفظ * بٽ_في_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	أرجع سائيز;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بنانشان ڊگهو ڳوليو_ايندڙ_ٻٽو_انگ(ثابت بنانشان ڊگهو *addr,
			    بنانشان ڊگهو سائيز, بنانشان ڊگهو وٿي)
{
	ثابت بنانشان ڊگهو *p = addr + BITOP_لفظ(وٿي);
	بنانشان ڊگهو نتيجو = وٿي & ~(بٽ_في_LONG-1);
	بنانشان ڊگهو tmp;

	إذا (وٿي >= سائيز)
		أرجع سائيز;
	سائيز -= نتيجو;
	وٿي %= بٽ_في_LONG;
	إذا (وٿي) {
		tmp = *(p++);
		tmp &= (~0UL << وٿي);
		إذا (سائيز < بٽ_في_LONG)
			اذهب مليو_پهريون;
		إذا (tmp)
			اذهب مليو_وچ;
		سائيز -= بٽ_في_LONG;
		نتيجو += بٽ_في_LONG;
	}
	طالما (سائيز & ~(بٽ_في_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب مليو_وچ;
		نتيجو += بٽ_في_LONG;
		سائيز -= بٽ_في_LONG;
	}
	إذا (!سائيز)
		أرجع نتيجو;
	tmp = *p;

مليو_پهريون:
	tmp &= (~0UL >> (بٽ_في_LONG - سائيز));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتيجو + سائيز;	/* Nope. */
مليو_وچ:
	أرجع نتيجو + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بنانشان ڊگهو ڳوليو_ايندڙ_zero_ٻٽو_انگ(ثابت بنانشان ڊگهو *addr,
				 بنانشان ڊگهو سائيز,
				 بنانشان ڊگهو وٿي)
{
	ثابت بنانشان ڊگهو *p = addr + BITOP_لفظ(وٿي);
	بنانشان ڊگهو نتيجو = وٿي & ~(بٽ_في_LONG-1);
	بنانشان ڊگهو tmp;

	إذا (وٿي >= سائيز)
		أرجع سائيز;
	سائيز -= نتيجو;
	وٿي %= بٽ_في_LONG;
	إذا (وٿي) {
		tmp = *(p++);
		tmp |= ~0UL >> (بٽ_في_LONG - وٿي);
		إذا (سائيز < بٽ_في_LONG)
			اذهب مليو_پهريون;
		إذا (~tmp)
			اذهب مليو_وچ;
		سائيز -= بٽ_في_LONG;
		نتيجو += بٽ_في_LONG;
	}
	طالما (سائيز & ~(بٽ_في_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب مليو_وچ;
		نتيجو += بٽ_في_LONG;
		سائيز -= بٽ_في_LONG;
	}
	إذا (!سائيز)
		أرجع نتيجو;
	tmp = *p;

مليو_پهريون:
	tmp |= ~0UL << سائيز;
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتيجو + سائيز;	/* Nope. */
مليو_وچ:
	أرجع نتيجو + sbi_ffz(tmp);
}
