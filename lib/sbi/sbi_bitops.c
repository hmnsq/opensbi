/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BITOP_ਸ਼ਬਦ(nr)		((nr) / ਬਿੱਟ_ਪ੍ਰਤੀ_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੱਭੋ_ਪਹਿਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *p = addr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	ਜਦੋਂ (ਮਾਪ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)) {
		ਜੇ ((tmp = *(p++)))
			ਜਾਓ ਮਿਲਿਆ;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ ਨਤੀਜਾ;

	tmp = (*p) & (~0UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - ਮਾਪ));
	ਜੇ (tmp == 0UL)		/* Are any bits set? */
		ਵਾਪਸ ਨਤੀਜਾ + ਮਾਪ;	/* Nope. */
ਮਿਲਿਆ:
	ਵਾਪਸ ਨਤੀਜਾ + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੱਭੋ_ਪਹਿਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr,
				  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *p = addr;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	ਜਦੋਂ (ਮਾਪ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)) {
		ਜੇ (~(tmp = *(p++)))
			ਜਾਓ ਮਿਲਿਆ;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ ਨਤੀਜਾ;

	tmp = (*p) | (~0UL << ਮਾਪ);
	ਜੇ (tmp == ~0UL)	/* Are any bits zero? */
		ਵਾਪਸ ਨਤੀਜਾ + ਮਾਪ;	/* Nope. */
ਮਿਲਿਆ:
	ਵਾਪਸ ਨਤੀਜਾ + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੱਭੋ_ਆਖਰੀ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸ਼ਬਦ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	/* Start at final word. */
	ਸ਼ਬਦ = ਮਾਪ / ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;

	/* Partial final word? */
	ਜੇ (ਮਾਪ & (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)) {
		tmp = (addr[ਸ਼ਬਦ] & (~0UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG
					 - (ਮਾਪ & (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)))));
		ਜੇ (tmp)
			ਜਾਓ ਮਿਲਿਆ;
	}

	ਜਦੋਂ (ਸ਼ਬਦ) {
		tmp = addr[--ਸ਼ਬਦ];
		ਜੇ (tmp) {
ਮਿਲਿਆ:
			ਵਾਪਸ ਸ਼ਬਦ * ਬਿੱਟ_ਪ੍ਰਤੀ_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ਵਾਪਸ ਮਾਪ;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੱਭੋ_ਅਗਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr,
			    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਿੱਥ)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *p = addr + BITOP_ਸ਼ਬਦ(ਵਿੱਥ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = ਵਿੱਥ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	ਜੇ (ਵਿੱਥ >= ਮਾਪ)
		ਵਾਪਸ ਮਾਪ;
	ਮਾਪ -= ਨਤੀਜਾ;
	ਵਿੱਥ %= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	ਜੇ (ਵਿੱਥ) {
		tmp = *(p++);
		tmp &= (~0UL << ਵਿੱਥ);
		ਜੇ (ਮਾਪ < ਬਿੱਟ_ਪ੍ਰਤੀ_LONG)
			ਜਾਓ ਮਿਲਿਆ_ਪਹਿਲਾ;
		ਜੇ (tmp)
			ਜਾਓ ਮਿਲਿਆ_ਵਿਚਕਾਰ;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜਦੋਂ (ਮਾਪ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)) {
		ਜੇ ((tmp = *(p++)))
			ਜਾਓ ਮਿਲਿਆ_ਵਿਚਕਾਰ;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ ਨਤੀਜਾ;
	tmp = *p;

ਮਿਲਿਆ_ਪਹਿਲਾ:
	tmp &= (~0UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - ਮਾਪ));
	ਜੇ (tmp == 0UL)		/* Are any bits set? */
		ਵਾਪਸ ਨਤੀਜਾ + ਮਾਪ;	/* Nope. */
ਮਿਲਿਆ_ਵਿਚਕਾਰ:
	ਵਾਪਸ ਨਤੀਜਾ + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੱਭੋ_ਅਗਲਾ_zero_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਿੱਥ)
{
	ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *p = addr + BITOP_ਸ਼ਬਦ(ਵਿੱਥ);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਤੀਜਾ = ਵਿੱਥ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp;

	ਜੇ (ਵਿੱਥ >= ਮਾਪ)
		ਵਾਪਸ ਮਾਪ;
	ਮਾਪ -= ਨਤੀਜਾ;
	ਵਿੱਥ %= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	ਜੇ (ਵਿੱਥ) {
		tmp = *(p++);
		tmp |= ~0UL >> (ਬਿੱਟ_ਪ੍ਰਤੀ_LONG - ਵਿੱਥ);
		ਜੇ (ਮਾਪ < ਬਿੱਟ_ਪ੍ਰਤੀ_LONG)
			ਜਾਓ ਮਿਲਿਆ_ਪਹਿਲਾ;
		ਜੇ (~tmp)
			ਜਾਓ ਮਿਲਿਆ_ਵਿਚਕਾਰ;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜਦੋਂ (ਮਾਪ & ~(ਬਿੱਟ_ਪ੍ਰਤੀ_LONG-1)) {
		ਜੇ (~(tmp = *(p++)))
			ਜਾਓ ਮਿਲਿਆ_ਵਿਚਕਾਰ;
		ਨਤੀਜਾ += ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
		ਮਾਪ -= ਬਿੱਟ_ਪ੍ਰਤੀ_LONG;
	}
	ਜੇ (!ਮਾਪ)
		ਵਾਪਸ ਨਤੀਜਾ;
	tmp = *p;

ਮਿਲਿਆ_ਪਹਿਲਾ:
	tmp |= ~0UL << ਮਾਪ;
	ਜੇ (tmp == ~0UL)	/* Are any bits zero? */
		ਵਾਪਸ ਨਤੀਜਾ + ਮਾਪ;	/* Nope. */
ਮਿਲਿਆ_ਵਿਚਕਾਰ:
	ਵਾਪਸ ਨਤੀਜਾ + sbi_ffz(tmp);
}
