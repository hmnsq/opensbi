/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(ثابت unsigned long *addr,
			     unsigned long size)
{
	ثابت unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	وقتی (size & ~(BITS_PER_LONG-1)) {
		اگر ((tmp = *(p++)))
			برو found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	اگر (!size)
		بازگشت result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	اگر (tmp == 0UL)		/* Are any bits set? */
		بازگشت result + size;	/* Nope. */
found:
	بازگشت result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ثابت unsigned long *addr,
				  unsigned long size)
{
	ثابت unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	وقتی (size & ~(BITS_PER_LONG-1)) {
		اگر (~(tmp = *(p++)))
			برو found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	اگر (!size)
		بازگشت result;

	tmp = (*p) | (~0UL << size);
	اگر (tmp == ~0UL)	/* Are any bits zero? */
		بازگشت result + size;	/* Nope. */
found:
	بازگشت result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ثابت unsigned long *addr,
			    unsigned long size)
{
	unsigned long واژگان;
	unsigned long tmp;

	/* Start at final word. */
	واژگان = size / BITS_PER_LONG;

	/* Partial final word? */
	اگر (size & (BITS_PER_LONG-1)) {
		tmp = (addr[واژگان] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		اگر (tmp)
			برو found;
	}

	وقتی (واژگان) {
		tmp = addr[--واژگان];
		اگر (tmp) {
found:
			بازگشت واژگان * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	بازگشت size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(ثابت unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	ثابت unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	اگر (offset >= size)
		بازگشت size;
	size -= result;
	offset %= BITS_PER_LONG;
	اگر (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		اگر (size < BITS_PER_LONG)
			برو found_first;
		اگر (tmp)
			برو found_میانه;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	وقتی (size & ~(BITS_PER_LONG-1)) {
		اگر ((tmp = *(p++)))
			برو found_میانه;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	اگر (!size)
		بازگشت result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	اگر (tmp == 0UL)		/* Are any bits set? */
		بازگشت result + size;	/* Nope. */
found_میانه:
	بازگشت result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(ثابت unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	ثابت unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	اگر (offset >= size)
		بازگشت size;
	size -= result;
	offset %= BITS_PER_LONG;
	اگر (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		اگر (size < BITS_PER_LONG)
			برو found_first;
		اگر (~tmp)
			برو found_میانه;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	وقتی (size & ~(BITS_PER_LONG-1)) {
		اگر (~(tmp = *(p++)))
			برو found_میانه;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	اگر (!size)
		بازگشت result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	اگر (tmp == ~0UL)	/* Are any bits zero? */
		بازگشت result + size;	/* Nope. */
found_میانه:
	بازگشت result + sbi_ffz(tmp);
}
