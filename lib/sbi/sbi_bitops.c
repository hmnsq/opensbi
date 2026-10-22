/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_bitops.h>

#define BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
unsigned long find_first_bit(സ്ഥിരം unsigned long *addr,
			     unsigned long size)
{
	സ്ഥിരം unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ആകുമ്പോൾ (size & ~(BITS_PER_LONG-1)) {
		എങ്കിൽ ((tmp = *(p++)))
			പോകുക found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	എങ്കിൽ (!size)
		തിരികെ result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	എങ്കിൽ (tmp == 0UL)		/* Are any bits set? */
		തിരികെ result + size;	/* Nope. */
found:
	തിരികെ result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
unsigned long find_first_zero_bit(സ്ഥിരം unsigned long *addr,
				  unsigned long size)
{
	സ്ഥിരം unsigned long *p = addr;
	unsigned long result = 0;
	unsigned long tmp;

	ആകുമ്പോൾ (size & ~(BITS_PER_LONG-1)) {
		എങ്കിൽ (~(tmp = *(p++)))
			പോകുക found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	എങ്കിൽ (!size)
		തിരികെ result;

	tmp = (*p) | (~0UL << size);
	എങ്കിൽ (tmp == ~0UL)	/* Are any bits zero? */
		തിരികെ result + size;	/* Nope. */
found:
	തിരികെ result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
unsigned long find_last_bit(സ്ഥിരം unsigned long *addr,
			    unsigned long size)
{
	unsigned long വാക്കുകൾ;
	unsigned long tmp;

	/* Start at final word. */
	വാക്കുകൾ = size / BITS_PER_LONG;

	/* Partial final word? */
	എങ്കിൽ (size & (BITS_PER_LONG-1)) {
		tmp = (addr[വാക്കുകൾ] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		എങ്കിൽ (tmp)
			പോകുക found;
	}

	ആകുമ്പോൾ (വാക്കുകൾ) {
		tmp = addr[--വാക്കുകൾ];
		എങ്കിൽ (tmp) {
found:
			തിരികെ വാക്കുകൾ * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	തിരികെ size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_bit(സ്ഥിരം unsigned long *addr,
			    unsigned long size, unsigned long offset)
{
	സ്ഥിരം unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	എങ്കിൽ (offset >= size)
		തിരികെ size;
	size -= result;
	offset %= BITS_PER_LONG;
	എങ്കിൽ (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		എങ്കിൽ (size < BITS_PER_LONG)
			പോകുക found_first;
		എങ്കിൽ (tmp)
			പോകുക found_നടുവ്;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ആകുമ്പോൾ (size & ~(BITS_PER_LONG-1)) {
		എങ്കിൽ ((tmp = *(p++)))
			പോകുക found_നടുവ്;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	എങ്കിൽ (!size)
		തിരികെ result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	എങ്കിൽ (tmp == 0UL)		/* Are any bits set? */
		തിരികെ result + size;	/* Nope. */
found_നടുവ്:
	തിരികെ result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
unsigned long find_next_zero_bit(സ്ഥിരം unsigned long *addr,
				 unsigned long size,
				 unsigned long offset)
{
	സ്ഥിരം unsigned long *p = addr + BITOP_WORD(offset);
	unsigned long result = offset & ~(BITS_PER_LONG-1);
	unsigned long tmp;

	എങ്കിൽ (offset >= size)
		തിരികെ size;
	size -= result;
	offset %= BITS_PER_LONG;
	എങ്കിൽ (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		എങ്കിൽ (size < BITS_PER_LONG)
			പോകുക found_first;
		എങ്കിൽ (~tmp)
			പോകുക found_നടുവ്;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	ആകുമ്പോൾ (size & ~(BITS_PER_LONG-1)) {
		എങ്കിൽ (~(tmp = *(p++)))
			പോകുക found_നടുവ്;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	എങ്കിൽ (!size)
		തിരികെ result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	എങ്കിൽ (tmp == ~0UL)	/* Are any bits zero? */
		തിരികെ result + size;	/* Nope. */
found_നടുവ്:
	തിരികെ result + sbi_ffz(tmp);
}
