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

	ขณะ (size & ~(BITS_PER_LONG-1)) {
		ถ้า ((tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ถ้า (!size)
		คืน result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	ถ้า (tmp == 0UL)		/* Are any bits set? */
		คืน result + size;	/* Nope. */
found:
	คืน result + sbi_ffs(tmp);
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

	ขณะ (size & ~(BITS_PER_LONG-1)) {
		ถ้า (~(tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ถ้า (!size)
		คืน result;

	tmp = (*p) | (~0UL << size);
	ถ้า (tmp == ~0UL)	/* Are any bits zero? */
		คืน result + size;	/* Nope. */
found:
	คืน result + sbi_ffz(tmp);
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
	ถ้า (size & (BITS_PER_LONG-1)) {
		tmp = (addr[words] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		ถ้า (tmp)
			goto found;
	}

	ขณะ (words) {
		tmp = addr[--words];
		ถ้า (tmp) {
found:
			คืน words * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	คืน size;
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

	ถ้า (offset >= size)
		คืน size;
	size -= result;
	offset %= BITS_PER_LONG;
	ถ้า (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		ถ้า (size < BITS_PER_LONG)
			goto found_first;
		ถ้า (tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ขณะ (size & ~(BITS_PER_LONG-1)) {
		ถ้า ((tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ถ้า (!size)
		คืน result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	ถ้า (tmp == 0UL)		/* Are any bits set? */
		คืน result + size;	/* Nope. */
found_middle:
	คืน result + sbi_ffs(tmp);
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

	ถ้า (offset >= size)
		คืน size;
	size -= result;
	offset %= BITS_PER_LONG;
	ถ้า (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		ถ้า (size < BITS_PER_LONG)
			goto found_first;
		ถ้า (~tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ขณะ (size & ~(BITS_PER_LONG-1)) {
		ถ้า (~(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ถ้า (!size)
		คืน result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	ถ้า (tmp == ~0UL)	/* Are any bits zero? */
		คืน result + size;	/* Nope. */
found_middle:
	คืน result + sbi_ffz(tmp);
}
