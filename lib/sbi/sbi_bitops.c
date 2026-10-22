/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_bitops.h>

#నిర్వచించు BITOP_మాట(nr)		((nr) / బిట్లు_చొప్పున_LONG)

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit.
 */
గుర్తులేని దీర్ఘ కనుగొను_మొదటి_బిట్టు(స్థిరరాశి గుర్తులేని దీర్ఘ *addr,
			     గుర్తులేని దీర్ఘ కొలత)
{
	స్థిరరాశి గుర్తులేని దీర్ఘ *p = addr;
	గుర్తులేని దీర్ఘ ఫలితం = 0;
	గుర్తులేని దీర్ఘ tmp;

	అయితే (కొలత & ~(బిట్లు_చొప్పున_LONG-1)) {
		ఒకవేళ ((tmp = *(p++)))
			వెళ్ళు దొరికింది;
		ఫలితం += బిట్లు_చొప్పున_LONG;
		కొలత -= బిట్లు_చొప్పున_LONG;
	}
	ఒకవేళ (!కొలత)
		తిరిగి ఫలితం;

	tmp = (*p) & (~0UL >> (బిట్లు_చొప్పున_LONG - కొలత));
	ఒకవేళ (tmp == 0UL)		/* Are any bits set? */
		తిరిగి ఫలితం + కొలత;	/* Nope. */
దొరికింది:
	తిరిగి ఫలితం + sbi_ffs(tmp);
}

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first cleared bit.
 */
గుర్తులేని దీర్ఘ కనుగొను_మొదటి_zero_బిట్టు(స్థిరరాశి గుర్తులేని దీర్ఘ *addr,
				  గుర్తులేని దీర్ఘ కొలత)
{
	స్థిరరాశి గుర్తులేని దీర్ఘ *p = addr;
	గుర్తులేని దీర్ఘ ఫలితం = 0;
	గుర్తులేని దీర్ఘ tmp;

	అయితే (కొలత & ~(బిట్లు_చొప్పున_LONG-1)) {
		ఒకవేళ (~(tmp = *(p++)))
			వెళ్ళు దొరికింది;
		ఫలితం += బిట్లు_చొప్పున_LONG;
		కొలత -= బిట్లు_చొప్పున_LONG;
	}
	ఒకవేళ (!కొలత)
		తిరిగి ఫలితం;

	tmp = (*p) | (~0UL << కొలత);
	ఒకవేళ (tmp == ~0UL)	/* Are any bits zero? */
		తిరిగి ఫలితం + కొలత;	/* Nope. */
దొరికింది:
	తిరిగి ఫలితం + sbi_ffz(tmp);
}

/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
 */
గుర్తులేని దీర్ఘ కనుగొను_చివరి_బిట్టు(స్థిరరాశి గుర్తులేని దీర్ఘ *addr,
			    గుర్తులేని దీర్ఘ కొలత)
{
	గుర్తులేని దీర్ఘ పదాలు;
	గుర్తులేని దీర్ఘ tmp;

	/* Start at final word. */
	పదాలు = కొలత / బిట్లు_చొప్పున_LONG;

	/* Partial final word? */
	ఒకవేళ (కొలత & (బిట్లు_చొప్పున_LONG-1)) {
		tmp = (addr[పదాలు] & (~0UL >> (బిట్లు_చొప్పున_LONG
					 - (కొలత & (బిట్లు_చొప్పున_LONG-1)))));
		ఒకవేళ (tmp)
			వెళ్ళు దొరికింది;
	}

	అయితే (పదాలు) {
		tmp = addr[--పదాలు];
		ఒకవేళ (tmp) {
దొరికింది:
			తిరిగి పదాలు * బిట్లు_చొప్పున_LONG + sbi_fls(tmp);
		}
	}

	/* Not found */
	తిరిగి కొలత;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
గుర్తులేని దీర్ఘ కనుగొను_తదుపరి_బిట్టు(స్థిరరాశి గుర్తులేని దీర్ఘ *addr,
			    గుర్తులేని దీర్ఘ కొలత, గుర్తులేని దీర్ఘ స్థానభ్రంశం)
{
	స్థిరరాశి గుర్తులేని దీర్ఘ *p = addr + BITOP_మాట(స్థానభ్రంశం);
	గుర్తులేని దీర్ఘ ఫలితం = స్థానభ్రంశం & ~(బిట్లు_చొప్పున_LONG-1);
	గుర్తులేని దీర్ఘ tmp;

	ఒకవేళ (స్థానభ్రంశం >= కొలత)
		తిరిగి కొలత;
	కొలత -= ఫలితం;
	స్థానభ్రంశం %= బిట్లు_చొప్పున_LONG;
	ఒకవేళ (స్థానభ్రంశం) {
		tmp = *(p++);
		tmp &= (~0UL << స్థానభ్రంశం);
		ఒకవేళ (కొలత < బిట్లు_చొప్పున_LONG)
			వెళ్ళు దొరికింది_మొదటి;
		ఒకవేళ (tmp)
			వెళ్ళు దొరికింది_మధ్య;
		కొలత -= బిట్లు_చొప్పున_LONG;
		ఫలితం += బిట్లు_చొప్పున_LONG;
	}
	అయితే (కొలత & ~(బిట్లు_చొప్పున_LONG-1)) {
		ఒకవేళ ((tmp = *(p++)))
			వెళ్ళు దొరికింది_మధ్య;
		ఫలితం += బిట్లు_చొప్పున_LONG;
		కొలత -= బిట్లు_చొప్పున_LONG;
	}
	ఒకవేళ (!కొలత)
		తిరిగి ఫలితం;
	tmp = *p;

దొరికింది_మొదటి:
	tmp &= (~0UL >> (బిట్లు_చొప్పున_LONG - కొలత));
	ఒకవేళ (tmp == 0UL)		/* Are any bits set? */
		తిరిగి ఫలితం + కొలత;	/* Nope. */
దొరికింది_మధ్య:
	తిరిగి ఫలితం + sbi_ffs(tmp);
}

/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 */
గుర్తులేని దీర్ఘ కనుగొను_తదుపరి_zero_బిట్టు(స్థిరరాశి గుర్తులేని దీర్ఘ *addr,
				 గుర్తులేని దీర్ఘ కొలత,
				 గుర్తులేని దీర్ఘ స్థానభ్రంశం)
{
	స్థిరరాశి గుర్తులేని దీర్ఘ *p = addr + BITOP_మాట(స్థానభ్రంశం);
	గుర్తులేని దీర్ఘ ఫలితం = స్థానభ్రంశం & ~(బిట్లు_చొప్పున_LONG-1);
	గుర్తులేని దీర్ఘ tmp;

	ఒకవేళ (స్థానభ్రంశం >= కొలత)
		తిరిగి కొలత;
	కొలత -= ఫలితం;
	స్థానభ్రంశం %= బిట్లు_చొప్పున_LONG;
	ఒకవేళ (స్థానభ్రంశం) {
		tmp = *(p++);
		tmp |= ~0UL >> (బిట్లు_చొప్పున_LONG - స్థానభ్రంశం);
		ఒకవేళ (కొలత < బిట్లు_చొప్పున_LONG)
			వెళ్ళు దొరికింది_మొదటి;
		ఒకవేళ (~tmp)
			వెళ్ళు దొరికింది_మధ్య;
		కొలత -= బిట్లు_చొప్పున_LONG;
		ఫలితం += బిట్లు_చొప్పున_LONG;
	}
	అయితే (కొలత & ~(బిట్లు_చొప్పున_LONG-1)) {
		ఒకవేళ (~(tmp = *(p++)))
			వెళ్ళు దొరికింది_మధ్య;
		ఫలితం += బిట్లు_చొప్పున_LONG;
		కొలత -= బిట్లు_చొప్పున_LONG;
	}
	ఒకవేళ (!కొలత)
		తిరిగి ఫలితం;
	tmp = *p;

దొరికింది_మొదటి:
	tmp |= ~0UL << కొలత;
	ఒకవేళ (tmp == ~0UL)	/* Are any bits zero? */
		తిరిగి ఫలితం + కొలత;	/* Nope. */
దొరికింది_మధ్య:
	తిరిగి ఫలితం + sbi_ffz(tmp);
}
