/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_bitops.h>

#تعریف_کرو BITOP_لفظ(nr)		((nr) / بٹ_فی_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
غیرعلامتی طویل ڈھونڈو_پہلا_ثنائی_ہندسہ(ثابت غیرعلامتی طویل *addr,
			     غیرعلامتی طویل سائز)
{
	ثابت غیرعلامتی طویل *p = addr;
	غیرعلامتی طویل نتیجہ = 0;
	غیرعلامتی طویل tmp;

	جب (سائز & ~(بٹ_فی_LONG-1)) {
		اگر ((tmp = *(p++)))
			جاؤ ملا;
		نتیجہ += بٹ_فی_LONG;
		سائز -= بٹ_فی_LONG;
	}
	اگر (!سائز)
		واپس نتیجہ;

	tmp = (*p) & (~0UL >> (بٹ_فی_LONG - سائز));
	اگر (tmp == 0UL)		/* Are any bits set? */
		واپس نتیجہ + سائز;	/* Nope. */
ملا:
	واپس نتیجہ + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
غیرعلامتی طویل ڈھونڈو_پہلا_zero_ثنائی_ہندسہ(ثابت غیرعلامتی طویل *addr,
				  غیرعلامتی طویل سائز)
{
	ثابت غیرعلامتی طویل *p = addr;
	غیرعلامتی طویل نتیجہ = 0;
	غیرعلامتی طویل tmp;

	جب (سائز & ~(بٹ_فی_LONG-1)) {
		اگر (~(tmp = *(p++)))
			جاؤ ملا;
		نتیجہ += بٹ_فی_LONG;
		سائز -= بٹ_فی_LONG;
	}
	اگر (!سائز)
		واپس نتیجہ;

	tmp = (*p) | (~0UL << سائز);
	اگر (tmp == ~0UL)	/* Are any bits zero? */
		واپس نتیجہ + سائز;	/* Nope. */
ملا:
	واپس نتیجہ + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
غیرعلامتی طویل ڈھونڈو_آخری_ثنائی_ہندسہ(ثابت غیرعلامتی طویل *addr,
			    غیرعلامتی طویل سائز)
{
	غیرعلامتی طویل الفاظ;
	غیرعلامتی طویل tmp;

	/* Start at final word. */
	الفاظ = سائز / بٹ_فی_LONG;

	/* Partial final word? */
	اگر (سائز & (بٹ_فی_LONG-1)) {
		tmp = (addr[الفاظ] & (~0UL >> (بٹ_فی_LONG
					 - (سائز & (بٹ_فی_LONG-1)))));
		اگر (tmp)
			جاؤ ملا;
	}

	جب (الفاظ) {
		tmp = addr[--الفاظ];
		اگر (tmp) {
ملا:
			واپس الفاظ * بٹ_فی_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	واپس سائز;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
غیرعلامتی طویل ڈھونڈو_اگلا_ثنائی_ہندسہ(ثابت غیرعلامتی طویل *addr,
			    غیرعلامتی طویل سائز, غیرعلامتی طویل انحراف)
{
	ثابت غیرعلامتی طویل *p = addr + BITOP_لفظ(انحراف);
	غیرعلامتی طویل نتیجہ = انحراف & ~(بٹ_فی_LONG-1);
	غیرعلامتی طویل tmp;

	اگر (انحراف >= سائز)
		واپس سائز;
	سائز -= نتیجہ;
	انحراف %= بٹ_فی_LONG;
	اگر (انحراف) {
		tmp = *(p++);
		tmp &= (~0UL << انحراف);
		اگر (سائز < بٹ_فی_LONG)
			جاؤ ملا_پہلا;
		اگر (tmp)
			جاؤ ملا_درمیان;
		سائز -= بٹ_فی_LONG;
		نتیجہ += بٹ_فی_LONG;
	}
	جب (سائز & ~(بٹ_فی_LONG-1)) {
		اگر ((tmp = *(p++)))
			جاؤ ملا_درمیان;
		نتیجہ += بٹ_فی_LONG;
		سائز -= بٹ_فی_LONG;
	}
	اگر (!سائز)
		واپس نتیجہ;
	tmp = *p;

ملا_پہلا:
	tmp &= (~0UL >> (بٹ_فی_LONG - سائز));
	اگر (tmp == 0UL)		/* Are any bits set? */
		واپس نتیجہ + سائز;	/* Nope. */
ملا_درمیان:
	واپس نتیجہ + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
غیرعلامتی طویل ڈھونڈو_اگلا_zero_ثنائی_ہندسہ(ثابت غیرعلامتی طویل *addr,
				 غیرعلامتی طویل سائز,
				 غیرعلامتی طویل انحراف)
{
	ثابت غیرعلامتی طویل *p = addr + BITOP_لفظ(انحراف);
	غیرعلامتی طویل نتیجہ = انحراف & ~(بٹ_فی_LONG-1);
	غیرعلامتی طویل tmp;

	اگر (انحراف >= سائز)
		واپس سائز;
	سائز -= نتیجہ;
	انحراف %= بٹ_فی_LONG;
	اگر (انحراف) {
		tmp = *(p++);
		tmp |= ~0UL >> (بٹ_فی_LONG - انحراف);
		اگر (سائز < بٹ_فی_LONG)
			جاؤ ملا_پہلا;
		اگر (~tmp)
			جاؤ ملا_درمیان;
		سائز -= بٹ_فی_LONG;
		نتیجہ += بٹ_فی_LONG;
	}
	جب (سائز & ~(بٹ_فی_LONG-1)) {
		اگر (~(tmp = *(p++)))
			جاؤ ملا_درمیان;
		نتیجہ += بٹ_فی_LONG;
		سائز -= بٹ_فی_LONG;
	}
	اگر (!سائز)
		واپس نتیجہ;
	tmp = *p;

ملا_پہلا:
	tmp |= ~0UL << سائز;
	اگر (tmp == ~0UL)	/* Are any bits zero? */
		واپس نتیجہ + سائز;	/* Nope. */
ملا_درمیان:
	واپس نتیجہ + sbi_ffz(tmp);
}
