/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(અચલ unsigned long *addr,
			     unsigned long size)
{
	અચલ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	જ્યારે (size & ~(BITS_PER_LONG-1)) {
		જો ((tmp = *(p++)))
			જાઓ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	જો (!size)
		પાછા result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	જો (tmp == 0UL)		/* Are any bits set? */
		પાછા result + size;	/* Nope. */
found:
	પાછા result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(અચલ unsigned long *addr,
				  unsigned long size)
{
	અચલ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	જ્યારે (size & ~(BITS_PER_LONG-1)) {
		જો (~(tmp = *(p++)))
			જાઓ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	જો (!size)
		પાછા result;

	tmp = (*p) | (~0UL << size);
	જો (tmp == ~0UL)	/* Are any bits zero? */
		પાછા result + size;	/* Nope. */
found:
	પાછા result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(અચલ unsigned long *addr,
			    unsigned long size)
{
	unsigned long શબ્દો;
	unsigned long tmp;

	/* Start at final word. */
	શબ્દો = size / BITS_PER_LONG;

	/* Partial final word? */
	જો (size & (BITS_PER_LONG-1)) {
		tmp = (addr[શબ્દો] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		જો (tmp)
			જાઓ found;
	}

	જ્યારે (શબ્દો) {
		tmp = addr[--શબ્દો];
		જો (tmp) {
found:
			પાછા શબ્દો * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	પાછા size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(અચલ unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	અચલ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	જો (offset >= size)
		પાછા size;
	size -= result;
	offset %= BITS_PER_LONG;
	જો (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		જો (size < BITS_PER_LONG)
			જાઓ found_first;
		જો (tmp)
			જાઓ found_મધ્ય;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	જ્યારે (size & ~(BITS_PER_LONG-1)) {
		જો ((tmp = *(p++)))
			જાઓ found_મધ્ય;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	જો (!size)
		પાછા result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	જો (tmp == 0UL)		/* Are any bits set? */
		પાછા result + size;	/* Nope. */
found_મધ્ય:
	પાછા result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(અચલ unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	અચલ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	જો (offset >= size)
		પાછા size;
	size -= result;
	offset %= BITS_PER_LONG;
	જો (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		જો (size < BITS_PER_LONG)
			જાઓ found_first;
		જો (~tmp)
			જાઓ found_મધ્ય;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	જ્યારે (size & ~(BITS_PER_LONG-1)) {
		જો (~(tmp = *(p++)))
			જાઓ found_મધ્ય;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	જો (!size)
		પાછા result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	જો (tmp == ~0UL)	/* Are any bits zero? */
		પાછા result + size;	/* Nope. */
found_મધ્ય:
	પાછા result + sbi_ffz(tmp);
}
