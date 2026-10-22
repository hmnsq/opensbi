/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(ଅଚଳ unsigned long *addr,
			     unsigned long size)
{
	ଅଚଳ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ଯେତେବେଳେ (size & ~(BITS_PER_LONG-1)) {
		ଯଦି ((tmp = *(p++)))
			ଯାଅ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ଯଦି (!size)
		ଫେରନ୍ତୁ result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	ଯଦି (tmp == 0UL)		/* Are any bits set? */
		ଫେରନ୍ତୁ result + size;	/* Nope. */
found:
	ଫେରନ୍ତୁ result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ଅଚଳ unsigned long *addr,
				  unsigned long size)
{
	ଅଚଳ unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ଯେତେବେଳେ (size & ~(BITS_PER_LONG-1)) {
		ଯଦି (~(tmp = *(p++)))
			ଯାଅ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ଯଦି (!size)
		ଫେରନ୍ତୁ result;

	tmp = (*p) | (~0UL << size);
	ଯଦି (tmp == ~0UL)	/* Are any bits zero? */
		ଫେରନ୍ତୁ result + size;	/* Nope. */
found:
	ଫେରନ୍ତୁ result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ଅଚଳ unsigned long *addr,
			    unsigned long size)
{
	unsigned long ଶବ୍ଦଗୁଡ଼ିକ;
	unsigned long tmp;

	/* Start at final word. */
	ଶବ୍ଦଗୁଡ଼ିକ = size / BITS_PER_LONG;

	/* Partial final word? */
	ଯଦି (size & (BITS_PER_LONG-1)) {
		tmp = (addr[ଶବ୍ଦଗୁଡ଼ିକ] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		ଯଦି (tmp)
			ଯାଅ found;
	}

	ଯେତେବେଳେ (ଶବ୍ଦଗୁଡ଼ିକ) {
		tmp = addr[--ଶବ୍ଦଗୁଡ଼ିକ];
		ଯଦି (tmp) {
found:
			ଫେରନ୍ତୁ ଶବ୍ଦଗୁଡ଼ିକ * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	ଫେରନ୍ତୁ size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(ଅଚଳ unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	ଅଚଳ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ଯଦି (offset >= size)
		ଫେରନ୍ତୁ size;
	size -= result;
	offset %= BITS_PER_LONG;
	ଯଦି (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		ଯଦି (size < BITS_PER_LONG)
			ଯାଅ found_first;
		ଯଦି (tmp)
			ଯାଅ found_ମଧ୍ୟ;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ଯେତେବେଳେ (size & ~(BITS_PER_LONG-1)) {
		ଯଦି ((tmp = *(p++)))
			ଯାଅ found_ମଧ୍ୟ;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ଯଦି (!size)
		ଫେରନ୍ତୁ result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	ଯଦି (tmp == 0UL)		/* Are any bits set? */
		ଫେରନ୍ତୁ result + size;	/* Nope. */
found_ମଧ୍ୟ:
	ଫେରନ୍ତୁ result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(ଅଚଳ unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	ଅଚଳ unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	ଯଦି (offset >= size)
		ଫେରନ୍ତୁ size;
	size -= result;
	offset %= BITS_PER_LONG;
	ଯଦି (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		ଯଦି (size < BITS_PER_LONG)
			ଯାଅ found_first;
		ଯଦି (~tmp)
			ଯାଅ found_ମଧ୍ୟ;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ଯେତେବେଳେ (size & ~(BITS_PER_LONG-1)) {
		ଯଦି (~(tmp = *(p++)))
			ଯାଅ found_ମଧ୍ୟ;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	ଯଦି (!size)
		ଫେରନ୍ତୁ result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	ଯଦି (tmp == ~0UL)	/* Are any bits zero? */
		ଫେରନ୍ତୁ result + size;	/* Nope. */
found_ମଧ୍ୟ:
	ଫେରନ୍ତୁ result + sbi_ffz(tmp);
}
