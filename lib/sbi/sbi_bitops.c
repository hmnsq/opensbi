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
unsigned long find_first_bit(ধ্রুবক unsigned long *addr,
			     unsigned long আকার_2)
{
	ধ্রুবক unsigned long *p = addr;
	unsigned long ফলাফল = 0;
	unsigned long tmp;

	যখন (আকার_2 & ~(BITS_PER_LONG-1)) {
		যদি ((tmp = *(p++)))
			যাও found;
		ফলাফল += BITS_PER_LONG;
		আকার_2 -= BITS_PER_LONG;
	}
	যদি (!আকার_2)
		ফেরত ফলাফল;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - আকার_2));
	যদি (tmp == 0UL)		/* Are any bits set? */
		ফেরত ফলাফল + আকার_2;	/* Nope. */
found:
	ফেরত ফলাফল + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ধ্রুবক unsigned long *addr,
				  unsigned long আকার_2)
{
	ধ্রুবক unsigned long *p = addr;
	unsigned long ফলাফল = 0;
	unsigned long tmp;

	যখন (আকার_2 & ~(BITS_PER_LONG-1)) {
		যদি (~(tmp = *(p++)))
			যাও found;
		ফলাফল += BITS_PER_LONG;
		আকার_2 -= BITS_PER_LONG;
	}
	যদি (!আকার_2)
		ফেরত ফলাফল;

	tmp = (*p) | (~0UL << আকার_2);
	যদি (tmp == ~0UL)	/* Are any bits zero? */
		ফেরত ফলাফল + আকার_2;	/* Nope. */
found:
	ফেরত ফলাফল + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ধ্রুবক unsigned long *addr,
			    unsigned long আকার_2)
{
	unsigned long শব্দগুলি;
	unsigned long tmp;

	/* Start at final word. */
	শব্দগুলি = আকার_2 / BITS_PER_LONG;

	/* Partial final word? */
	যদি (আকার_2 & (BITS_PER_LONG-1)) {
		tmp = (addr[শব্দগুলি] & (~0UL >> (BITS_PER_LONG
					 - (আকার_2 & (BITS_PER_LONG-1)))));
		যদি (tmp)
			যাও found;
	}

	যখন (শব্দগুলি) {
		tmp = addr[--শব্দগুলি];
		যদি (tmp) {
found:
			ফেরত শব্দগুলি * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ফেরত আকার_2;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_পরবর্তী_bit(ধ্রুবক unsigned long *addr,
			    unsigned long আকার_2, unsigned long offset)
{
	ধ্রুবক unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long ফলাফল = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	যদি (offset >= আকার_2)
		ফেরত আকার_2;
	আকার_2 -= ফলাফল;
	offset %= BITS_PER_LONG;
	যদি (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		যদি (আকার_2 < BITS_PER_LONG)
			যাও found_first;
		যদি (tmp)
			যাও found_মধ্য;
		আকার_2 -= BITS_PER_LONG;
		ফলাফল += BITS_PER_LONG;
	}
	যখন (আকার_2 & ~(BITS_PER_LONG-1)) {
		যদি ((tmp = *(p++)))
			যাও found_মধ্য;
		ফলাফল += BITS_PER_LONG;
		আকার_2 -= BITS_PER_LONG;
	}
	যদি (!আকার_2)
		ফেরত ফলাফল;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - আকার_2));
	যদি (tmp == 0UL)		/* Are any bits set? */
		ফেরত ফলাফল + আকার_2;	/* Nope. */
found_মধ্য:
	ফেরত ফলাফল + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_পরবর্তী_zero_bit(ধ্রুবক unsigned long *addr,
				 unsigned long আকার_2,
				 unsigned long offset)
{
	ধ্রুবক unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long ফলাফল = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	যদি (offset >= আকার_2)
		ফেরত আকার_2;
	আকার_2 -= ফলাফল;
	offset %= BITS_PER_LONG;
	যদি (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		যদি (আকার_2 < BITS_PER_LONG)
			যাও found_first;
		যদি (~tmp)
			যাও found_মধ্য;
		আকার_2 -= BITS_PER_LONG;
		ফলাফল += BITS_PER_LONG;
	}
	যখন (আকার_2 & ~(BITS_PER_LONG-1)) {
		যদি (~(tmp = *(p++)))
			যাও found_মধ্য;
		ফলাফল += BITS_PER_LONG;
		আকার_2 -= BITS_PER_LONG;
	}
	যদি (!আকার_2)
		ফেরত ফলাফল;
	tmp = *p;

found_first:
	tmp |= ~0UL << আকার_2;
	যদি (tmp == ~0UL)	/* Are any bits zero? */
		ফেরত ফলাফল + আকার_2;	/* Nope. */
found_মধ্য:
	ফেরত ফলাফল + sbi_ffz(tmp);
}
