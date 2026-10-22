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

	སྐབས (size & ~(BITS_PER_LONG-1)) {
		གལ་སྲིད ((tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	གལ་སྲིད (!size)
		ལོག result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	གལ་སྲིད (tmp == 0UL)		/* Are any bits set? */
		ལོག result + size;	/* Nope. */
found:
	ལོག result + sbi_ffs(tmp);
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

	སྐབས (size & ~(BITS_PER_LONG-1)) {
		གལ་སྲིད (~(tmp = *(p++)))
			goto found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	གལ་སྲིད (!size)
		ལོག result;

	tmp = (*p) | (~0UL << size);
	གལ་སྲིད (tmp == ~0UL)	/* Are any bits zero? */
		ལོག result + size;	/* Nope. */
found:
	ལོག result + sbi_ffz(tmp);
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
	གལ་སྲིད (size & (BITS_PER_LONG-1)) {
		tmp = (addr[words] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		གལ་སྲིད (tmp)
			goto found;
	}

	སྐབས (words) {
		tmp = addr[--words];
		གལ་སྲིད (tmp) {
found:
			ལོག words * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ལོག size;
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

	གལ་སྲིད (offset >= size)
		ལོག size;
	size -= result;
	offset %= BITS_PER_LONG;
	གལ་སྲིད (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		གལ་སྲིད (size < BITS_PER_LONG)
			goto found_first;
		གལ་སྲིད (tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	སྐབས (size & ~(BITS_PER_LONG-1)) {
		གལ་སྲིད ((tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	གལ་སྲིད (!size)
		ལོག result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	གལ་སྲིད (tmp == 0UL)		/* Are any bits set? */
		ལོག result + size;	/* Nope. */
found_middle:
	ལོག result + sbi_ffs(tmp);
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

	གལ་སྲིད (offset >= size)
		ལོག size;
	size -= result;
	offset %= BITS_PER_LONG;
	གལ་སྲིད (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		གལ་སྲིད (size < BITS_PER_LONG)
			goto found_first;
		གལ་སྲིད (~tmp)
			goto found_middle;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	སྐབས (size & ~(BITS_PER_LONG-1)) {
		གལ་སྲིད (~(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	གལ་སྲིད (!size)
		ལོག result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	གལ་སྲིད (tmp == ~0UL)	/* Are any bits zero? */
		ལོག result + size;	/* Nope. */
found_middle:
	ལོག result + sbi_ffz(tmp);
}
