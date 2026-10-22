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

	طالما (size & ~(BITS_PER_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	إذا (!size)
		أرجع result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع result + size;	/* Nope. */
found:
	أرجع result + sbi_ffs(tmp);
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

	طالما (size & ~(BITS_PER_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	إذا (!size)
		أرجع result;

	tmp = (*p) | (~0UL << size);
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع result + size;	/* Nope. */
found:
	أرجع result + sbi_ffz(tmp);
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
	unsigned long كلموم;
	unsigned long tmp;

	/* Start at final word. */
	كلموم = size / BITS_PER_LONG;

	/* Partial final word? */
	إذا (size & (BITS_PER_LONG-1)) {
		tmp = (addr[كلموم] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		إذا (tmp)
			اذهب found;
	}

	طالما (كلموم) {
		tmp = addr[--كلموم];
		إذا (tmp) {
found:
			أرجع كلموم * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	أرجع size;
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

	إذا (offset >= size)
		أرجع size;
	size -= result;
	offset %= BITS_PER_LONG;
	إذا (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		إذا (size < BITS_PER_LONG)
			اذهب found_first;
		إذا (tmp)
			اذهب found_ڟكيا;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	طالما (size & ~(BITS_PER_LONG-1)) {
		إذا ((tmp = *(p++)))
			اذهب found_ڟكيا;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	إذا (!size)
		أرجع result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	إذا (tmp == 0UL)		/* Are any bits set? */
		أرجع result + size;	/* Nope. */
found_ڟكيا:
	أرجع result + sbi_ffs(tmp);
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

	إذا (offset >= size)
		أرجع size;
	size -= result;
	offset %= BITS_PER_LONG;
	إذا (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		إذا (size < BITS_PER_LONG)
			اذهب found_first;
		إذا (~tmp)
			اذهب found_ڟكيا;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	طالما (size & ~(BITS_PER_LONG-1)) {
		إذا (~(tmp = *(p++)))
			اذهب found_ڟكيا;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	إذا (!size)
		أرجع result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	إذا (tmp == ~0UL)	/* Are any bits zero? */
		أرجع result + size;	/* Nope. */
found_ڟكيا:
	أرجع result + sbi_ffz(tmp);
}
