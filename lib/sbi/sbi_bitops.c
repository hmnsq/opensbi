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

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(ಧ್ರುವ unsigned long *addr,
			     unsigned long size)
{
	ಧ್ರುವ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ಯಾವತ್ (size & ~(BITS_PER_LONG-1)) {
		ಯದಿ ((tmp = *(p++)))
			ಗಚ್ಛ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ಯದಿ (!size)
		ಪ್ರತ್ಯಾಗಮ result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	ಯದಿ (tmp == 0UL)		/* Are any bits set? */
		ಪ್ರತ್ಯಾಗಮ result + size;	/* Nope. */
found:
	ಪ್ರತ್ಯಾಗಮ result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ಧ್ರುವ unsigned long *addr,
				  unsigned long size)
{
	ಧ್ರುವ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ಯಾವತ್ (size & ~(BITS_PER_LONG-1)) {
		ಯದಿ (~(tmp = *(p++)))
			ಗಚ್ಛ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ಯದಿ (!size)
		ಪ್ರತ್ಯಾಗಮ result;

	tmp = (*p) | (~0UL << size);
	ಯದಿ (tmp == ~0UL)	/* Are any bits zero? */
		ಪ್ರತ್ಯಾಗಮ result + size;	/* Nope. */
found:
	ಪ್ರತ್ಯಾಗಮ result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ಧ್ರುವ unsigned long *addr,
			    unsigned long size)
{
	unsigned long ಪದಗಳು;
	unsigned long tmp;

	/* Start at final word. */
	ಪದಗಳು = size / BITS_PER_LONG;

	/* Partial final word? */
	ಯದಿ (size & (BITS_PER_LONG-1)) {
		tmp = (addr[ಪದಗಳು] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		ಯದಿ (tmp)
			ಗಚ್ಛ found;
	}

	ಯಾವತ್ (ಪದಗಳು) {
		tmp = addr[--ಪದಗಳು];
		ಯದಿ (tmp) {
found:
			ಪ್ರತ್ಯಾಗಮ ಪದಗಳು * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ಪ್ರತ್ಯಾಗಮ size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(ಧ್ರುವ unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	ಧ್ರುವ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ಯದಿ (offset >= size)
		ಪ್ರತ್ಯಾಗಮ size;
	size -= result;
	offset %= BITS_PER_LONG;
	ಯದಿ (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		ಯದಿ (size < BITS_PER_LONG)
			ಗಚ್ಛ found_first;
		ಯದಿ (tmp)
			ಗಚ್ಛ found_ಮಧ್ಯ;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ಯಾವತ್ (size & ~(BITS_PER_LONG-1)) {
		ಯದಿ ((tmp = *(p++)))
			ಗಚ್ಛ found_ಮಧ್ಯ;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ಯದಿ (!size)
		ಪ್ರತ್ಯಾಗಮ result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	ಯದಿ (tmp == 0UL)		/* Are any bits set? */
		ಪ್ರತ್ಯಾಗಮ result + size;	/* Nope. */
found_ಮಧ್ಯ:
	ಪ್ರತ್ಯಾಗಮ result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(ಧ್ರುವ unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	ಧ್ರುವ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ಯದಿ (offset >= size)
		ಪ್ರತ್ಯಾಗಮ size;
	size -= result;
	offset %= BITS_PER_LONG;
	ಯದಿ (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		ಯದಿ (size < BITS_PER_LONG)
			ಗಚ್ಛ found_first;
		ಯದಿ (~tmp)
			ಗಚ್ಛ found_ಮಧ್ಯ;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ಯಾವತ್ (size & ~(BITS_PER_LONG-1)) {
		ಯದಿ (~(tmp = *(p++)))
			ಗಚ್ಛ found_ಮಧ್ಯ;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ಯದಿ (!size)
		ಪ್ರತ್ಯಾಗಮ result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	ಯದಿ (tmp == ~0UL)	/* Are any bits zero? */
		ಪ್ರತ್ಯಾಗಮ result + size;	/* Nope. */
found_ಮಧ್ಯ:
	ಪ್ರತ್ಯಾಗಮ result + sbi_ffz(tmp);
}
