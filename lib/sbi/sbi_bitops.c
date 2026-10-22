/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(மாறிலி unsigned long *addr,
			     unsigned long size)
{
	மாறிலி unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	போது (size & ~(BITS_PER_LONG-1)) {
		எனில் ((tmp = *(p++)))
			செல் found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	எனில் (!size)
		திரும்பு result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	எனில் (tmp == 0UL)		/* Are any bits set? */
		திரும்பு result + size;	/* Nope. */
found:
	திரும்பு result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(மாறிலி unsigned long *addr,
				  unsigned long size)
{
	மாறிலி unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	போது (size & ~(BITS_PER_LONG-1)) {
		எனில் (~(tmp = *(p++)))
			செல் found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	எனில் (!size)
		திரும்பு result;

	tmp = (*p) | (~0UL << size);
	எனில் (tmp == ~0UL)	/* Are any bits zero? */
		திரும்பு result + size;	/* Nope. */
found:
	திரும்பு result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(மாறிலி unsigned long *addr,
			    unsigned long size)
{
	unsigned long சொற்கள்;
	unsigned long tmp;

	/* Start at final word. */
	சொற்கள் = size / BITS_PER_LONG;

	/* Partial final word? */
	எனில் (size & (BITS_PER_LONG-1)) {
		tmp = (addr[சொற்கள்] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		எனில் (tmp)
			செல் found;
	}

	போது (சொற்கள்) {
		tmp = addr[--சொற்கள்];
		எனில் (tmp) {
found:
			திரும்பு சொற்கள் * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	திரும்பு size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(மாறிலி unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	மாறிலி unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	எனில் (offset >= size)
		திரும்பு size;
	size -= result;
	offset %= BITS_PER_LONG;
	எனில் (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		எனில் (size < BITS_PER_LONG)
			செல் found_first;
		எனில் (tmp)
			செல் found_நடு;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	போது (size & ~(BITS_PER_LONG-1)) {
		எனில் ((tmp = *(p++)))
			செல் found_நடு;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	எனில் (!size)
		திரும்பு result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	எனில் (tmp == 0UL)		/* Are any bits set? */
		திரும்பு result + size;	/* Nope. */
found_நடு:
	திரும்பு result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(மாறிலி unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	மாறிலி unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	எனில் (offset >= size)
		திரும்பு size;
	size -= result;
	offset %= BITS_PER_LONG;
	எனில் (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		எனில் (size < BITS_PER_LONG)
			செல் found_first;
		எனில் (~tmp)
			செல் found_நடு;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	போது (size & ~(BITS_PER_LONG-1)) {
		எனில் (~(tmp = *(p++)))
			செல் found_நடு;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	எனில் (!size)
		திரும்பு result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	எனில் (tmp == ~0UL)	/* Are any bits zero? */
		திரும்பு result + size;	/* Nope. */
found_நடு:
	திரும்பு result + sbi_ffz(tmp);
}
