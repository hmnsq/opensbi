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
अचिन्हित दीर्घ find_first_bit(स्थिरांक अचिन्हित दीर्घ *addr,
			     अचिन्हित दीर्घ size)
{
	स्थिरांक अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ result = 0;
	अचिन्हित दीर्घ tmp;

	पर्यंत (size & ~(BITS_PER_LONG-1)) {
		जर ((tmp = *(p++)))
			जा found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	जर (!size)
		परत result;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - size));
	जर (tmp == 0UL)		/* Are any bits set? */
		परत result + size;	/* Nope. */
found:
	परत result + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
अचिन्हित दीर्घ find_first_zero_bit(स्थिरांक अचिन्हित दीर्घ *addr,
				  अचिन्हित दीर्घ size)
{
	स्थिरांक अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ result = 0;
	अचिन्हित दीर्घ tmp;

	पर्यंत (size & ~(BITS_PER_LONG-1)) {
		जर (~(tmp = *(p++)))
			जा found;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	जर (!size)
		परत result;

	tmp = (*p) | (~0UL << size);
	जर (tmp == ~0UL)	/* Are any bits zero? */
		परत result + size;	/* Nope. */
found:
	परत result + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
अचिन्हित दीर्घ find_last_bit(स्थिरांक अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ size)
{
	अचिन्हित दीर्घ शब्द;
	अचिन्हित दीर्घ tmp;

	/* Start at final word. */
	शब्द = size / BITS_PER_LONG;

	/* Partial final word? */
	जर (size & (BITS_PER_LONG-1)) {
		tmp = (addr[शब्द] & (~0UL >> (BITS_PER_LONG
					 - (size & (BITS_PER_LONG-1)))));
		जर (tmp)
			जा found;
	}

	पर्यंत (शब्द) {
		tmp = addr[--शब्द];
		जर (tmp) {
found:
			परत शब्द * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	परत size;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_next_bit(स्थिरांक अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ size, अचिन्हित दीर्घ offset)
{
	स्थिरांक अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ result = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	जर (offset >= size)
		परत size;
	size -= result;
	offset %= BITS_PER_LONG;
	जर (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		जर (size < BITS_PER_LONG)
			जा found_first;
		जर (tmp)
			जा found_मधला;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	पर्यंत (size & ~(BITS_PER_LONG-1)) {
		जर ((tmp = *(p++)))
			जा found_मधला;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	जर (!size)
		परत result;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - size));
	जर (tmp == 0UL)		/* Are any bits set? */
		परत result + size;	/* Nope. */
found_मधला:
	परत result + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_next_zero_bit(स्थिरांक अचिन्हित दीर्घ *addr,
				 अचिन्हित दीर्घ size,
				 अचिन्हित दीर्घ offset)
{
	स्थिरांक अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ result = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	जर (offset >= size)
		परत size;
	size -= result;
	offset %= BITS_PER_LONG;
	जर (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		जर (size < BITS_PER_LONG)
			जा found_first;
		जर (~tmp)
			जा found_मधला;
		size -= BITS_PER_LONG;
		result += BITS_PER_LONG;
	}
	पर्यंत (size & ~(BITS_PER_LONG-1)) {
		जर (~(tmp = *(p++)))
			जा found_मधला;
		result += BITS_PER_LONG;
		size -= BITS_PER_LONG;
	}
	जर (!size)
		परत result;
	tmp = *p;

found_first:
	tmp |= ~0UL << size;
	जर (tmp == ~0UL)	/* Are any bits zero? */
		परत result + size;	/* Nope. */
found_मधला:
	परत result + sbi_ffz(tmp);
}
