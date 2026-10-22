/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_bitops.h>

#घोषणा BITOP_WORD(nr)		((nr) / BITS_PER_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
अचिन्हित दीर्घ find_first_bit(स्थिर अचिन्हित दीर्घ *addr,
			     अचिन्हित दीर्घ आकार)
{
	स्थिर अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ परिणाम = 0;
	अचिन्हित दीर्घ tmp;

	जबतक (आकार & ~(BITS_PER_LONG-1)) {
		अगर ((tmp = *(p++)))
			जाओ found;
		परिणाम += BITS_PER_LONG;
		आकार -= BITS_PER_LONG;
	}
	अगर (!आकार)
		वापस परिणाम;

	tmp = (*p) & (~0UL >> (BITS_PER_LONG - आकार));
	अगर (tmp == 0UL)		/* Are any bits set? */
		वापस परिणाम + आकार;	/* Nope. */
found:
	वापस परिणाम + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
अचिन्हित दीर्घ find_first_zero_bit(स्थिर अचिन्हित दीर्घ *addr,
				  अचिन्हित दीर्घ आकार)
{
	स्थिर अचिन्हित दीर्घ *p = addr;
	अचिन्हित दीर्घ परिणाम = 0;
	अचिन्हित दीर्घ tmp;

	जबतक (आकार & ~(BITS_PER_LONG-1)) {
		अगर (~(tmp = *(p++)))
			जाओ found;
		परिणाम += BITS_PER_LONG;
		आकार -= BITS_PER_LONG;
	}
	अगर (!आकार)
		वापस परिणाम;

	tmp = (*p) | (~0UL << आकार);
	अगर (tmp == ~0UL)	/* Are any bits zero? */
		वापस परिणाम + आकार;	/* Nope. */
found:
	वापस परिणाम + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
अचिन्हित दीर्घ find_last_bit(स्थिर अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ आकार)
{
	अचिन्हित दीर्घ शब्दगण;
	अचिन्हित दीर्घ tmp;

	/* Start at final word. */
	शब्दगण = आकार / BITS_PER_LONG;

	/* Partial final word? */
	अगर (आकार & (BITS_PER_LONG-1)) {
		tmp = (addr[शब्दगण] & (~0UL >> (BITS_PER_LONG
					 - (आकार & (BITS_PER_LONG-1)))));
		अगर (tmp)
			जाओ found;
	}

	जबतक (शब्दगण) {
		tmp = addr[--शब्दगण];
		अगर (tmp) {
found:
			वापस शब्दगण * BITS_PER_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	वापस आकार;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_अग्र_bit(स्थिर अचिन्हित दीर्घ *addr,
			    अचिन्हित दीर्घ आकार, अचिन्हित दीर्घ offset)
{
	स्थिर अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ परिणाम = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	अगर (offset >= आकार)
		वापस आकार;
	आकार -= परिणाम;
	offset %= BITS_PER_LONG;
	अगर (offset) {
		tmp = *(p++);
		tmp &= (~0UL << offset);
		अगर (आकार < BITS_PER_LONG)
			जाओ found_first;
		अगर (tmp)
			जाओ found_मध्य;
		आकार -= BITS_PER_LONG;
		परिणाम += BITS_PER_LONG;
	}
	जबतक (आकार & ~(BITS_PER_LONG-1)) {
		अगर ((tmp = *(p++)))
			जाओ found_मध्य;
		परिणाम += BITS_PER_LONG;
		आकार -= BITS_PER_LONG;
	}
	अगर (!आकार)
		वापस परिणाम;
	tmp = *p;

found_first:
	tmp &= (~0UL >> (BITS_PER_LONG - आकार));
	अगर (tmp == 0UL)		/* Are any bits set? */
		वापस परिणाम + आकार;	/* Nope. */
found_मध्य:
	वापस परिणाम + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिन्हित दीर्घ find_अग्र_zero_bit(स्थिर अचिन्हित दीर्घ *addr,
				 अचिन्हित दीर्घ आकार,
				 अचिन्हित दीर्घ offset)
{
	स्थिर अचिन्हित दीर्घ *p = addr + BITOP_WORD(offset);
	अचिन्हित दीर्घ परिणाम = offset & ~(BITS_PER_LONG-1);
	अचिन्हित दीर्घ tmp;

	अगर (offset >= आकार)
		वापस आकार;
	आकार -= परिणाम;
	offset %= BITS_PER_LONG;
	अगर (offset) {
		tmp = *(p++);
		tmp |= ~0UL >> (BITS_PER_LONG - offset);
		अगर (आकार < BITS_PER_LONG)
			जाओ found_first;
		अगर (~tmp)
			जाओ found_मध्य;
		आकार -= BITS_PER_LONG;
		परिणाम += BITS_PER_LONG;
	}
	जबतक (आकार & ~(BITS_PER_LONG-1)) {
		अगर (~(tmp = *(p++)))
			जाओ found_मध्य;
		परिणाम += BITS_PER_LONG;
		आकार -= BITS_PER_LONG;
	}
	अगर (!आकार)
		वापस परिणाम;
	tmp = *p;

found_first:
	tmp |= ~0UL << आकार;
	अगर (tmp == ~0UL)	/* Are any bits zero? */
		वापस परिणाम + आकार;	/* Nope. */
found_मध्य:
	वापस परिणाम + sbi_ffz(tmp);
}
