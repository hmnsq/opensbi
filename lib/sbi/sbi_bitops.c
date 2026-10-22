/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>

#ವ್ಯಾಖ್ಯಾನಿಸು BITOP_ಶಬ್ದ(nr)		((nr) / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr,
			     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *p = addr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	ಆಗ (ಗಾತ್ರ_2 & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)) {
		ಆದರೆ ((tmp = *(p++)))
			ಹೋಗು ಸಿಕ್ಕಿತು;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

	tmp = (*p) & (~0UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - ಗಾತ್ರ_2));
	ಆದರೆ (tmp == 0UL)		/* Are any bits set? */
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + ಗಾತ್ರ_2;	/* Nope. */
ಸಿಕ್ಕಿತು:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಂಡುಹಿಡಿ_ಮೊದಲ_zero_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr,
				  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *p = addr;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	ಆಗ (ಗಾತ್ರ_2 & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)) {
		ಆದರೆ (~(tmp = *(p++)))
			ಹೋಗು ಸಿಕ್ಕಿತು;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

	tmp = (*p) | (~0UL << ಗಾತ್ರ_2);
	ಆದರೆ (tmp == ~0UL)	/* Are any bits zero? */
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + ಗಾತ್ರ_2;	/* Nope. */
ಸಿಕ್ಕಿತು:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr,
			    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪದಗಳು;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	/* Start at final word. */
	ಪದಗಳು = ಗಾತ್ರ_2 / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;

	/* Partial final word? */
	ಆದರೆ (ಗಾತ್ರ_2 & (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)) {
		tmp = (addr[ಪದಗಳು] & (~0UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG
					 - (ಗಾತ್ರ_2 & (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)))));
		ಆದರೆ (tmp)
			ಹೋಗು ಸಿಕ್ಕಿತು;
	}

	ಆಗ (ಪದಗಳು) {
		tmp = addr[--ಪದಗಳು];
		ಆದರೆ (tmp) {
ಸಿಕ್ಕಿತು:
			ಹಿಂತಿರುಗಿ ಪದಗಳು * ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr,
			    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಾನಾಂತರ)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *p = addr + BITOP_ಶಬ್ದ(ಸ್ಥಾನಾಂತರ);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = ಸ್ಥಾನಾಂತರ & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	ಆದರೆ (ಸ್ಥಾನಾಂತರ >= ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
	ಗಾತ್ರ_2 -= ಫಲಿತಾಂಶ;
	ಸ್ಥಾನಾಂತರ %= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	ಆದರೆ (ಸ್ಥಾನಾಂತರ) {
		tmp = *(p++);
		tmp &= (~0UL << ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (ಗಾತ್ರ_2 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG)
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮೊದಲ;
		ಆದರೆ (tmp)
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮಧ್ಯ;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆಗ (ಗಾತ್ರ_2 & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)) {
		ಆದರೆ ((tmp = *(p++)))
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮಧ್ಯ;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	tmp = *p;

ಸಿಕ್ಕಿತು_ಮೊದಲ:
	tmp &= (~0UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - ಗಾತ್ರ_2));
	ಆದರೆ (tmp == 0UL)		/* Are any bits set? */
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + ಗಾತ್ರ_2;	/* Nope. */
ಸಿಕ್ಕಿತು_ಮಧ್ಯ:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_zero_ಬಿಟ್(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2,
				 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಾನಾಂತರ)
{
	ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *p = addr + BITOP_ಶಬ್ದ(ಸ್ಥಾನಾಂತರ);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಫಲಿತಾಂಶ = ಸ್ಥಾನಾಂತರ & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1);
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp;

	ಆದರೆ (ಸ್ಥಾನಾಂತರ >= ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
	ಗಾತ್ರ_2 -= ಫಲಿತಾಂಶ;
	ಸ್ಥಾನಾಂತರ %= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	ಆದರೆ (ಸ್ಥಾನಾಂತರ) {
		tmp = *(p++);
		tmp |= ~0UL >> (ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG - ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (ಗಾತ್ರ_2 < ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG)
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮೊದಲ;
		ಆದರೆ (~tmp)
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮಧ್ಯ;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆಗ (ಗಾತ್ರ_2 & ~(ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG-1)) {
		ಆದರೆ (~(tmp = *(p++)))
			ಹೋಗು ಸಿಕ್ಕಿತು_ಮಧ್ಯ;
		ಫಲಿತಾಂಶ += ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
		ಗಾತ್ರ_2 -= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_LONG;
	}
	ಆದರೆ (!ಗಾತ್ರ_2)
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	tmp = *p;

ಸಿಕ್ಕಿತು_ಮೊದಲ:
	tmp |= ~0UL << ಗಾತ್ರ_2;
	ಆದರೆ (tmp == ~0UL)	/* Are any bits zero? */
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + ಗಾತ್ರ_2;	/* Nope. */
ಸಿಕ್ಕಿತು_ಮಧ್ಯ:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ + sbi_ffz(tmp);
}
