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

#परिभाषय BITOP_WORD(nr)		((nr) / द्वयङ्काः_प्रत्येकम्_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
अचिह्नित दीर्घ find_first_bit(ध्रुव अचिह्नित दीर्घ *addr,
			     अचिह्नित दीर्घ आकार)
{
	ध्रुव अचिह्नित दीर्घ *p = addr;
	अचिह्नित दीर्घ परिणाम = 0;
	अचिह्नित दीर्घ tmp;

	यावत् (आकार & ~(द्वयङ्काः_प्रत्येकम्_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ प्राप्तम्;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यदि (!आकार)
		प्रत्यागम परिणाम;

	tmp = (*p) & (~0UL >> (द्वयङ्काः_प्रत्येकम्_LONG - आकार));
	यदि (tmp == 0UL)		/* Are any bits set? */
		प्रत्यागम परिणाम + आकार;	/* Nope. */
प्राप्तम्:
	प्रत्यागम परिणाम + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
अचिह्नित दीर्घ find_first_zero_bit(ध्रुव अचिह्नित दीर्घ *addr,
				  अचिह्नित दीर्घ आकार)
{
	ध्रुव अचिह्नित दीर्घ *p = addr;
	अचिह्नित दीर्घ परिणाम = 0;
	अचिह्नित दीर्घ tmp;

	यावत् (आकार & ~(द्वयङ्काः_प्रत्येकम्_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ प्राप्तम्;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यदि (!आकार)
		प्रत्यागम परिणाम;

	tmp = (*p) | (~0UL << आकार);
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		प्रत्यागम परिणाम + आकार;	/* Nope. */
प्राप्तम्:
	प्रत्यागम परिणाम + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
अचिह्नित दीर्घ find_चरमः_bit(ध्रुव अचिह्नित दीर्घ *addr,
			    अचिह्नित दीर्घ आकार)
{
	अचिह्नित दीर्घ शब्दाः;
	अचिह्नित दीर्घ tmp;

	/* Start at final word. */
	शब्दाः = आकार / द्वयङ्काः_प्रत्येकम्_LONG;

	/* Partial final word? */
	यदि (आकार & (द्वयङ्काः_प्रत्येकम्_LONG-1)) {
		tmp = (addr[शब्दाः] & (~0UL >> (द्वयङ्काः_प्रत्येकम्_LONG
					 - (आकार & (द्वयङ्काः_प्रत्येकम्_LONG-1)))));
		यदि (tmp)
			गच्छ प्राप्तम्;
	}

	यावत् (शब्दाः) {
		tmp = addr[--शब्दाः];
		यदि (tmp) {
प्राप्तम्:
			प्रत्यागम शब्दाः * द्वयङ्काः_प्रत्येकम्_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	प्रत्यागम आकार;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिह्नित दीर्घ find_अग्र_bit(ध्रुव अचिह्नित दीर्घ *addr,
			    अचिह्नित दीर्घ आकार, अचिह्नित दीर्घ विस्थापनम्)
{
	ध्रुव अचिह्नित दीर्घ *p = addr + BITOP_WORD(विस्थापनम्);
	अचिह्नित दीर्घ परिणाम = विस्थापनम् & ~(द्वयङ्काः_प्रत्येकम्_LONG-1);
	अचिह्नित दीर्घ tmp;

	यदि (विस्थापनम् >= आकार)
		प्रत्यागम आकार;
	आकार -= परिणाम;
	विस्थापनम् %= द्वयङ्काः_प्रत्येकम्_LONG;
	यदि (विस्थापनम्) {
		tmp = *(p++);
		tmp &= (~0UL << विस्थापनम्);
		यदि (आकार < द्वयङ्काः_प्रत्येकम्_LONG)
			गच्छ प्राप्तम्_first;
		यदि (tmp)
			गच्छ प्राप्तम्_मध्यः;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यावत् (आकार & ~(द्वयङ्काः_प्रत्येकम्_LONG-1)) {
		यदि ((tmp = *(p++)))
			गच्छ प्राप्तम्_मध्यः;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यदि (!आकार)
		प्रत्यागम परिणाम;
	tmp = *p;

प्राप्तम्_first:
	tmp &= (~0UL >> (द्वयङ्काः_प्रत्येकम्_LONG - आकार));
	यदि (tmp == 0UL)		/* Are any bits set? */
		प्रत्यागम परिणाम + आकार;	/* Nope. */
प्राप्तम्_मध्यः:
	प्रत्यागम परिणाम + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
अचिह्नित दीर्घ find_अग्र_zero_bit(ध्रुव अचिह्नित दीर्घ *addr,
				 अचिह्नित दीर्घ आकार,
				 अचिह्नित दीर्घ विस्थापनम्)
{
	ध्रुव अचिह्नित दीर्घ *p = addr + BITOP_WORD(विस्थापनम्);
	अचिह्नित दीर्घ परिणाम = विस्थापनम् & ~(द्वयङ्काः_प्रत्येकम्_LONG-1);
	अचिह्नित दीर्घ tmp;

	यदि (विस्थापनम् >= आकार)
		प्रत्यागम आकार;
	आकार -= परिणाम;
	विस्थापनम् %= द्वयङ्काः_प्रत्येकम्_LONG;
	यदि (विस्थापनम्) {
		tmp = *(p++);
		tmp |= ~0UL >> (द्वयङ्काः_प्रत्येकम्_LONG - विस्थापनम्);
		यदि (आकार < द्वयङ्काः_प्रत्येकम्_LONG)
			गच्छ प्राप्तम्_first;
		यदि (~tmp)
			गच्छ प्राप्तम्_मध्यः;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यावत् (आकार & ~(द्वयङ्काः_प्रत्येकम्_LONG-1)) {
		यदि (~(tmp = *(p++)))
			गच्छ प्राप्तम्_मध्यः;
		परिणाम += द्वयङ्काः_प्रत्येकम्_LONG;
		आकार -= द्वयङ्काः_प्रत्येकम्_LONG;
	}
	यदि (!आकार)
		प्रत्यागम परिणाम;
	tmp = *p;

प्राप्तम्_first:
	tmp |= ~0UL << आकार;
	यदि (tmp == ~0UL)	/* Are any bits zero? */
		प्रत्यागम परिणाम + आकार;	/* Nope. */
प्राप्तम्_मध्यः:
	प्रत्यागम परिणाम + sbi_ffz(tmp);
}
