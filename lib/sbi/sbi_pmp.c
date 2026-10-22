/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_pmp.h>

స్థిర గుర్తులేని దీర్ఘ ctz(గుర్తులేని దీర్ఘ x)
{
	గుర్తులేని దీర్ఘ ret = 0;

	ఒకవేళ (x == 0)
		తిరిగి 8 * పరిమాణం(x);

	అయితే (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	తిరిగి ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ఒకవేళ (pmp->cfg & PMP_A)
		తిరిగి true;

	తిరిగి false;
}

పూర్ణాంకం sbi_pmp_encode(pmp_t *pmp, గుర్తులేని దీర్ఘ prot, గుర్తులేని దీర్ఘ addr,
		    గుర్తులేని దీర్ఘ log2len)
{
	/* check parameters */
	ఒకవేళ (log2len > __riscv_xlen || log2len < PMP_జరుపు)
		తిరిగి SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_జరుపు) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ఒకవేళ (log2len == PMP_జరుపు) {
		pmp->addr = (addr >> PMP_జరుపు);
	} లేకపోతే {
		ఒకవేళ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} లేకపోతే {
			గుర్తులేని దీర్ఘ addrmask;
			addrmask = (1UL << (log2len - PMP_జరుపు)) - 1;
			pmp->addr = ((addr >> PMP_జరుపు) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	తిరిగి SBI_OK;
}

పూర్ణాంకం sbi_pmp_decode(pmp_t *pmp, గుర్తులేని దీర్ఘ *prot_బహిర్గతం, గుర్తులేని దీర్ఘ *addr_బహిర్గతం,
		    గుర్తులేని దీర్ఘ *log2len)
{
	గుర్తులేని దీర్ఘ prot;
	గుర్తులేని దీర్ఘ t1, addr, len;

	/* check parameters */
	ఒకవేళ (!prot_బహిర్గతం || !addr_బహిర్గతం || !log2len)
		తిరిగి SBI_EINVAL;
	*prot_బహిర్గతం = *addr_బహిర్గతం = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ఒకవేళ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ఒకవేళ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} లేకపోతే {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_జరుపు;
			len	= (t1 + PMP_జరుపు + 1);
		}
	} లేకపోతే {
		addr	= pmp->addr << PMP_జరుపు;
		len	= PMP_జరుపు;
	}

	/* return details */
	*prot_బహిర్గతం    = prot;
	*addr_బహిర్గతం    = addr;
	*log2len     = len;

	తిరిగి SBI_OK;
}
