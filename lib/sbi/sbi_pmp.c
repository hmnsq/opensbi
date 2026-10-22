/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_encoding.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_pmp.h>

ସ୍ଥିର unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	ଯଦି (x == 0)
		ଫେରନ୍ତୁ 8 * ଆକାର(x);

	ଯେତେବେଳେ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ଫେରନ୍ତୁ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ଯଦି (pmp->cfg & PMP_A)
		ଫେରନ୍ତୁ true;

	ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	ଯଦି (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ଯଦି (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} ଅନ୍ୟଥା {
		ଯଦି (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ଅନ୍ୟଥା {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ଫେରନ୍ତୁ SBI_OK;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	ଯଦି (!prot_out || !addr_out || !log2len)
		ଫେରନ୍ତୁ SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ଯଦି ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ଯଦି (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ଅନ୍ୟଥା {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} ଅନ୍ୟଥା {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ଫେରନ୍ତୁ SBI_OK;
}
