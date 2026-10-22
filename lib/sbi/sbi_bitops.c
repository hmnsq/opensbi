/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(ध्रुव unsigned long *addr,
			     unsigned long size)
{
	ध्रुव unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	यावत् (size & ~(BITS_PER_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		प्रत्यागम result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	यदि (tmp == 0UL)		/* Are any bits set? */
		प्रत्यागम result + size;	/* Nope. */
found:
	प्रत्यागम result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(ध्रुव unsigned long *addr,
				  unsigned long size)
{
	ध्रुव unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	यावत् (size & ~(BITS_PER_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		प्रत्यागम result;

	tmp = (*p) | (~0UL << size);
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		प्रत्यागम result + size;	/* Nope. */
found:
	प्रत्यागम result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(ध्रुव unsigned long *addr,
			    unsigned long size)
{
	unsigned long शब्दगण;
	unsigned long tmp;

	/* Start at final word. */
	शब्दगण = size / BITS_PER_LONG;

	/* Partial final word? */
	यदि (size & (BITS_PER_LONG-1)) {
		tmp = (addr[शब्दगण] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		यदि (tmp)
			गच्छ found;
	}

	यावत् (शब्दगण) {
		tmp = addr[--शब्दगण];
		यदि (tmp) {
found:
			प्रत्यागम शब्दगण * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	प्रत्यागम size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(ध्रुव unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	ध्रुव unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	यदि (offset >= size)
		प्रत्यागम size;
	size -= result;
	offset %= BITS_PER_LONG;
	यदि (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		यदि (size < BITS_PER_LONG)
			गच्छ found_first;
		यदि (tmp)
			गच्छ found_मध्य;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	यावत् (size & ~(BITS_PER_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ found_मध्य;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		प्रत्यागम result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	यदि (tmp == 0UL)		/* Are any bits set? */
		प्रत्यागम result + size;	/* Nope. */
found_मध्य:
	प्रत्यागम result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(ध्रुव unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	ध्रुव unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	यदि (offset >= size)
		प्रत्यागम size;
	size -= result;
	offset %= BITS_PER_LONG;
	यदि (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		यदि (size < BITS_PER_LONG)
			गच्छ found_first;
		यदि (~tmp)
			गच्छ found_मध्य;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	यावत् (size & ~(BITS_PER_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ found_मध्य;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		प्रत्यागम result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		प्रत्यागम result + size;	/* Nope. */
found_मध्य:
	प्रत्यागम result + sbi_ffz(tmp);
}
