/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(ধ্রুব unsigned long *addr,
			     unsigned long size)
{
	ধ্রুব unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	যাবত্ (size & ~(BITS_PER_LONG-1)) {
		যদি ((tmp = *(p++)))
			গচ্ছ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	যদি (!size)
		প্রত্যাগম result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	যদি (tmp == 0UL)		/* Are any bits set? */
		প্রত্যাগম result + size;	/* Nope. */
found:
	প্রত্যাগম result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ধ্রুব unsigned long *addr,
				  unsigned long size)
{
	ধ্রুব unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	যাবত্ (size & ~(BITS_PER_LONG-1)) {
		যদি (~(tmp = *(p++)))
			গচ্ছ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	যদি (!size)
		প্রত্যাগম result;

	tmp = (*p) | (~0UL << size);
	যদি (tmp == ~0UL)	/* Are any bits zero? */
		প্রত্যাগম result + size;	/* Nope. */
found:
	প্রত্যাগম result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ধ্রুব unsigned long *addr,
			    unsigned long size)
{
	unsigned long শব্দগুলি;
	unsigned long tmp;

	/* Start at final word. */
	শব্দগুলি = size / BITS_PER_LONG;

	/* Partial final word? */
	যদি (size & (BITS_PER_LONG-1)) {
		tmp = (addr[শব্দগুলি] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		যদি (tmp)
			গচ্ছ found;
	}

	যাবত্ (শব্দগুলি) {
		tmp = addr[--শব্দগুলি];
		যদি (tmp) {
found:
			প্রত্যাগম শব্দগুলি * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	প্রত্যাগম size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(ধ্রুব unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	ধ্রুব unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	যদি (offset >= size)
		প্রত্যাগম size;
	size -= result;
	offset %= BITS_PER_LONG;
	যদি (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		যদি (size < BITS_PER_LONG)
			গচ্ছ found_first;
		যদি (tmp)
			গচ্ছ found_মধ্য;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	যাবত্ (size & ~(BITS_PER_LONG-1)) {
		যদি ((tmp = *(p++)))
			গচ্ছ found_মধ্য;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	যদি (!size)
		প্রত্যাগম result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	যদি (tmp == 0UL)		/* Are any bits set? */
		প্রত্যাগম result + size;	/* Nope. */
found_মধ্য:
	প্রত্যাগম result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(ধ্রুব unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	ধ্রুব unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	যদি (offset >= size)
		প্রত্যাগম size;
	size -= result;
	offset %= BITS_PER_LONG;
	যদি (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		যদি (size < BITS_PER_LONG)
			গচ্ছ found_first;
		যদি (~tmp)
			গচ্ছ found_মধ্য;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	যাবত্ (size & ~(BITS_PER_LONG-1)) {
		যদি (~(tmp = *(p++)))
			গচ্ছ found_মধ্য;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	যদি (!size)
		প্রত্যাগম result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	যদি (tmp == ~0UL)	/* Are any bits zero? */
		প্রত্যাগম result + size;	/* Nope. */
found_মধ্য:
	প্রত্যাগম result + sbi_ffz(tmp);
}
