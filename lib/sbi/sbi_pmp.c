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

	အကယ်၍ (x == 0)
		ပြန်ပေး 8 * parimaa_na(x);

	နေစဉ် (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ပြန်ပေး ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	အကယ်၍ (pmp->cfg & PMP_A)
		ပြန်ပေး true;

	ပြန်ပေး false;
}

ကိန်းပြည့် sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	အကယ်၍ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ပြန်ပေး SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	အကယ်၍ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} မဟုတ်ပါက {
		အကယ်၍ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} မဟုတ်ပါက {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ပြန်ပေး SBI_OK;
}

ကိန်းပြည့် sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	အကယ်၍ (!prot_out || !addr_out || !log2len)
		ပြန်ပေး SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	အကယ်၍ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		အကယ်၍ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} မဟုတ်ပါက {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} မဟုတ်ပါက {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ပြန်ပေး SBI_OK;
}
