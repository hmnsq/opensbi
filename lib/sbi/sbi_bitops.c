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
अचिन्हित दीर्घ find_first_bit(अचर अचिन्हित दीर्घ *addr,
			     अचिन्हित दीर्घ size)
{
	अचर अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ result = 0;
	अचिन्हित दीर्घ tmp;

	जबसम्म (size & ~(BITS_PER_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		फर्काऊ result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	यदि (tmp == 0UL)		/* Are any bits set? */
		फर्काऊ result + size;	/* Nope. */
found:
	फर्काऊ result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
अचिन्हित दीर्घ find_first_zero_bit(अचर अचिन्हित दीर्घ *addr,
				  अचिन्हित दीर्घ size)
{
	अचर अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ result = 0;
	अचिन्हित दीर्घ tmp;

	जबसम्म (size & ~(BITS_PER_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		फर्काऊ result;

	tmp = (*p) | (~0UL << size);
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		फर्काऊ result + size;	/* Nope. */
found:
	फर्काऊ result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
अचिन्हित दीर्घ find_last_bit(अचर अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ size)
{
	अचिन्हित दीर्घ शब्दहरू;
	अचिन्हित दीर्घ tmp;

	/* Start at final word. */
	शब्दहरू = size / BITS_PER_LONG;

	/* Partial final word? */
	यदि (size & (BITS_PER_LONG-1)) {
		tmp = (addr[शब्दहरू] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		यदि (tmp)
			गच्छ found;
	}

	जबसम्म (शब्दहरू) {
		tmp = addr[--शब्दहरू];
		यदि (tmp) {
found:
			फर्काऊ शब्दहरू * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	फर्काऊ size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_next_bit(अचर अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ size, अचिन्हित दीर्घ offset)
{
	अचर अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ result = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	यदि (offset >= size)
		फर्काऊ size;
	size -= result;
	offset %= BITS_PER_LONG;
	यदि (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		यदि (size < BITS_PER_LONG)
			गच्छ found_first;
		यदि (tmp)
			गच्छ found_बीच;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	जबसम्म (size & ~(BITS_PER_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ found_बीच;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		फर्काऊ result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	यदि (tmp == 0UL)		/* Are any bits set? */
		फर्काऊ result + size;	/* Nope. */
found_बीच:
	फर्काऊ result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_next_zero_bit(अचर अचिन्हित दीर्घ *addr,
				 अचिन्हित दीर्घ size,
				 अचिन्हित दीर्घ offset)
{
	अचर अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ result = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	यदि (offset >= size)
		फर्काऊ size;
	size -= result;
	offset %= BITS_PER_LONG;
	यदि (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		यदि (size < BITS_PER_LONG)
			गच्छ found_first;
		यदि (~tmp)
			गच्छ found_बीच;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	जबसम्म (size & ~(BITS_PER_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ found_बीच;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	यदि (!size)
		फर्काऊ result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		फर्काऊ result + size;	/* Nope. */
found_बीच:
	फर्काऊ result + sbi_ffz(tmp);
}
