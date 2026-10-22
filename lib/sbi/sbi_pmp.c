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

	ប្រសិនបើ (x == 0)
		ត្រឡប់ 8 * parimaa_na(x);

	ខណៈពេល (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ត្រឡប់ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ប្រសិនបើ (pmp->cfg & PMP_A)
		ត្រឡប់ true;

	ត្រឡប់ false;
}

ចំនួនគត់ sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	ប្រសិនបើ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ត្រឡប់ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ប្រសិនបើ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} បើមិនដូច្នេះ {
		ប្រសិនបើ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} បើមិនដូច្នេះ {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ត្រឡប់ SBI_OK;
}

ចំនួនគត់ sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	ប្រសិនបើ (!prot_out || !addr_out || !log2len)
		ត្រឡប់ SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ប្រសិនបើ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ប្រសិនបើ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} បើមិនដូច្នេះ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} បើមិនដូច្នេះ {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ត្រឡប់ SBI_OK;
}
