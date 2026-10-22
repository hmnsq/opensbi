/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_encoding.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_pmp.h>

સ્થિર unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	જો (x == 0)
		પાછા 8 * કદ(x);

	જ્યારે (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	પાછા ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	જો (pmp->cfg & PMP_A)
		પાછા true;

	પાછા false;
}

પૂર્ણાંક sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	જો (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		પાછા SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	જો (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} નહીંતર {
		જો (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} નહીંતર {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	પાછા SBI_OK;
}

પૂર્ણાંક sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	જો (!prot_out || !addr_out || !log2len)
		પાછા SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	જો ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		જો (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} નહીંતર {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} નહીંતર {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	પાછા SBI_OK;
}
