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

	याव (size & ~(BITS_PER_LONG-1)) {
		सचे ((tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	सचे (!size)
		पटिनिवत्त result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	सचे (tmp == 0UL)		/* Are any bits set? */
		पटिनिवत्त result + size;	/* Nope. */
found:
	पटिनिवत्त result + sbi_ffs(tmp);
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

	याव (size & ~(BITS_PER_LONG-1)) {
		सचे (~(tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	सचे (!size)
		पटिनिवत्त result;

	tmp = (*p) | (~0UL << size);
	सचे (tmp == ~0UL)	/* Are any bits zero? */
		पटिनिवत्त result + size;	/* Nope. */
found:
	पटिनिवत्त result + sbi_ffz(tmp);
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
	unsigned long शब्दाः;
	unsigned long tmp;

	/* Start at final word. */
	शब्दाः = size / BITS_PER_LONG;

	/* Partial final word? */
	सचे (size & (BITS_PER_LONG-1)) {
		tmp = (addr[शब्दाः] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		सचे (tmp)
			गच्छ found;
	}

	याव (शब्दाः) {
		tmp = addr[--शब्दाः];
		सचे (tmp) {
found:
			पटिनिवत्त शब्दाः * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	पटिनिवत्त size;
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

	सचे (offset >= size)
		पटिनिवत्त size;
	size -= result;
	offset %= BITS_PER_LONG;
	सचे (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		सचे (size < BITS_PER_LONG)
			गच्छ found_first;
		सचे (tmp)
			गच्छ found_मध्यः;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	याव (size & ~(BITS_PER_LONG-1)) {
		सचे ((tmp = *(p++)))
			गच्छ found_मध्यः;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	सचे (!size)
		पटिनिवत्त result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	सचे (tmp == 0UL)		/* Are any bits set? */
		पटिनिवत्त result + size;	/* Nope. */
found_मध्यः:
	पटिनिवत्त result + sbi_ffs(tmp);
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

	सचे (offset >= size)
		पटिनिवत्त size;
	size -= result;
	offset %= BITS_PER_LONG;
	सचे (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		सचे (size < BITS_PER_LONG)
			गच्छ found_first;
		सचे (~tmp)
			गच्छ found_मध्यः;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	याव (size & ~(BITS_PER_LONG-1)) {
		सचे (~(tmp = *(p++)))
			गच्छ found_मध्यः;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	सचे (!size)
		पटिनिवत्त result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	सचे (tmp == ~0UL)	/* Are any bits zero? */
		पटिनिवत्त result + size;	/* Nope. */
found_मध्यः:
	पटिनिवत्त result + sbi_ffz(tmp);
}
