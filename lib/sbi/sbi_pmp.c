/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_pmp.h>

static unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	ᰀᰧ (x == 0)
		ᰀᰧᰌ 8 * parimaa_na(x);

	ᰀᰬ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ᰀᰧᰌ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ᰀᰧ (pmp->cfg & PMP_A)
		ᰀᰧᰌ true;

	ᰀᰧᰌ false;
}

puur_naa_nka sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	ᰀᰧ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ᰀᰧᰌ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ᰀᰧ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} ᰀᰧᰀ {
		ᰀᰧ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ᰀᰧᰀ {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ᰀᰧᰌ SBI_OK;
}

puur_naa_nka sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	ᰀᰧ (!prot_out || !addr_out || !log2len)
		ᰀᰧᰌ SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ᰀᰧ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ᰀᰧ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ᰀᰧᰀ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} ᰀᰧᰀ {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ᰀᰧᰌ SBI_OK;
}
