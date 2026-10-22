/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(const unsigned long *addr,
			     unsigned long size)
{
	const unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	док (size & ~(BITS_PER_LONG-1)) {
		ако ((tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ако (!size)
		врати result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	ако (tmp == 0UL)		/* Are any bits set? */
		врати result + size;	/* Nope. */
found:
	врати result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(const unsigned long *addr,
				  unsigned long size)
{
	const unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	док (size & ~(BITS_PER_LONG-1)) {
		ако (~(tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ако (!size)
		врати result;

	tmp = (*p) | (~0UL << size);
	ако (tmp == ~0UL)	/* Are any bits zero? */
		врати result + size;	/* Nope. */
found:
	врати result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(const unsigned long *addr,
			    unsigned long size)
{
	unsigned long words;
	unsigned long tmp;

	/* Start at final word. */
	words = size / BITS_PER_LONG;

	/* Partial final word? */
	ако (size & (BITS_PER_LONG-1)) {
		tmp = (addr[words] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		ако (tmp)
			goto found;
	}

	док (words) {
		tmp = addr[--words];
		ако (tmp) {
found:
			врати words * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	врати size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(const unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	const unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ако (offset >= size)
		врати size;
	size -= result;
	offset %= BITS_PER_LONG;
	ако (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		ако (size < BITS_PER_LONG)
			goto found_first;
		ако (tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	док (size & ~(BITS_PER_LONG-1)) {
		ако ((tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ако (!size)
		врати result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	ако (tmp == 0UL)		/* Are any bits set? */
		врати result + size;	/* Nope. */
found_middle:
	врати result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(const unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	const unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ако (offset >= size)
		врати size;
	size -= result;
	offset %= BITS_PER_LONG;
	ако (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		ако (size < BITS_PER_LONG)
			goto found_first;
		ако (~tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	док (size & ~(BITS_PER_LONG-1)) {
		ако (~(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ако (!size)
		врати result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	ако (tmp == ~0UL)	/* Are any bits zero? */
		врати result + size;	/* Nope. */
found_middle:
	врати result + sbi_ffz(tmp);
}
