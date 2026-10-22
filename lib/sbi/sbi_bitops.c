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

#تعریف_کرو BITOP_لفظ(nr)		((nr) / بٹ_فی_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
بنانشانی لما لبھو_پہلا_ثنائی_ہندسہ(ثابت بنانشانی لما *addr,
			     بنانشانی لما ناپ)
{
	ثابت بنانشانی لما *p = addr;
	بنانشانی لما نتیجہ = 0;
	بنانشانی لما tmp;

	طالما (ناپ & ~(بٹ_فی_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب لبھیا;
		نتیجہ += بٹ_فی_LONG;
		ناپ -= بٹ_فی_LONG;
	}
	إذا (!ناپ)
		أرجع نتیجہ;

	tmp = (*p) & (~0UL >> (بٹ_فی_LONG - ناپ));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتیجہ + ناپ;	/* Nope. */
لبھیا:
	أرجع نتیجہ + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
بنانشانی لما لبھو_پہلا_zero_ثنائی_ہندسہ(ثابت بنانشانی لما *addr,
				  بنانشانی لما ناپ)
{
	ثابت بنانشانی لما *p = addr;
	بنانشانی لما نتیجہ = 0;
	بنانشانی لما tmp;

	طالما (ناپ & ~(بٹ_فی_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب لبھیا;
		نتیجہ += بٹ_فی_LONG;
		ناپ -= بٹ_فی_LONG;
	}
	إذا (!ناپ)
		أرجع نتیجہ;

	tmp = (*p) | (~0UL << ناپ);
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتیجہ + ناپ;	/* Nope. */
لبھیا:
	أرجع نتیجہ + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
بنانشانی لما لبھو_آخری_ثنائی_ہندسہ(ثابت بنانشانی لما *addr,
			    بنانشانی لما ناپ)
{
	بنانشانی لما الفاظ;
	بنانشانی لما tmp;

	/* Start at final word. */
	الفاظ = ناپ / بٹ_فی_LONG;

	/* Partial final word? */
	إذا (ناپ & (بٹ_فی_LONG-1)) {
		tmp = (addr[الفاظ] & (~0UL >> (بٹ_فی_LONG
					 - (ناپ & (بٹ_فی_LONG-1)))));
		إذا (tmp)
			اذهب لبھیا;
	}

	طالما (الفاظ) {
		tmp = addr[--الفاظ];
		إذا (tmp) {
لبھیا:
			أرجع الفاظ * بٹ_فی_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	أرجع ناپ;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بنانشانی لما لبھو_اگلا_ثنائی_ہندسہ(ثابت بنانشانی لما *addr,
			    بنانشانی لما ناپ, بنانشانی لما وتھ)
{
	ثابت بنانشانی لما *p = addr + BITOP_لفظ(وتھ);
	بنانشانی لما نتیجہ = وتھ & ~(بٹ_فی_LONG-1);
	بنانشانی لما tmp;

	إذا (وتھ >= ناپ)
		أرجع ناپ;
	ناپ -= نتیجہ;
	وتھ %= بٹ_فی_LONG;
	إذا (وتھ) {
		tmp = *(p++);
		tmp &= (~0UL << وتھ);
		إذا (ناپ < بٹ_فی_LONG)
			اذهب لبھیا_پہلا;
		إذا (tmp)
			اذهب لبھیا_درمیان;
		ناپ -= بٹ_فی_LONG;
		نتیجہ += بٹ_فی_LONG;
	}
	طالما (ناپ & ~(بٹ_فی_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب لبھیا_درمیان;
		نتیجہ += بٹ_فی_LONG;
		ناپ -= بٹ_فی_LONG;
	}
	إذا (!ناپ)
		أرجع نتیجہ;
	tmp = *p;

لبھیا_پہلا:
	tmp &= (~0UL >> (بٹ_فی_LONG - ناپ));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع نتیجہ + ناپ;	/* Nope. */
لبھیا_درمیان:
	أرجع نتیجہ + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
بنانشانی لما لبھو_اگلا_zero_ثنائی_ہندسہ(ثابت بنانشانی لما *addr,
				 بنانشانی لما ناپ,
				 بنانشانی لما وتھ)
{
	ثابت بنانشانی لما *p = addr + BITOP_لفظ(وتھ);
	بنانشانی لما نتیجہ = وتھ & ~(بٹ_فی_LONG-1);
	بنانشانی لما tmp;

	إذا (وتھ >= ناپ)
		أرجع ناپ;
	ناپ -= نتیجہ;
	وتھ %= بٹ_فی_LONG;
	إذا (وتھ) {
		tmp = *(p++);
		tmp |= ~0UL >> (بٹ_فی_LONG - وتھ);
		إذا (ناپ < بٹ_فی_LONG)
			اذهب لبھیا_پہلا;
		إذا (~tmp)
			اذهب لبھیا_درمیان;
		ناپ -= بٹ_فی_LONG;
		نتیجہ += بٹ_فی_LONG;
	}
	طالما (ناپ & ~(بٹ_فی_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب لبھیا_درمیان;
		نتیجہ += بٹ_فی_LONG;
		ناپ -= بٹ_فی_LONG;
	}
	إذا (!ناپ)
		أرجع نتیجہ;
	tmp = *p;

لبھیا_پہلا:
	tmp |= ~0UL << ناپ;
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع نتیجہ + ناپ;	/* Nope. */
لبھیا_درمیان:
	أرجع نتیجہ + sbi_ffz(tmp);
}
