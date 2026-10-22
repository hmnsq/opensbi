/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_pmp.h>

ساكن unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	ܐܢ (x == 0)
		ܦܢܐ 8 * حجم(x);

	ܟܡܐ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ܦܢܐ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ܐܢ (pmp->cfg & PMP_A)
		ܦܢܐ true;

	ܦܢܐ false;
}

صحيح sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	ܐܢ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ܦܢܐ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ܐܢ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} ܐܚܪܢܐ {
		ܐܢ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ܐܚܪܢܐ {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ܦܢܐ SBI_OK;
}

صحيح sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	ܐܢ (!prot_out || !addr_out || !log2len)
		ܦܢܐ SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ܐܢ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ܐܢ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ܐܚܪܢܐ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} ܐܚܪܢܐ {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ܦܢܐ SBI_OK;
}
