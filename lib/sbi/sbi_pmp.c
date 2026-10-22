/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_encoding.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_pmp.h>

நிலையான unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	எனில் (x == 0)
		திரும்பு 8 * அளவு(x);

	போது (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	திரும்பு ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	எனில் (pmp->cfg & PMP_A)
		திரும்பு true;

	திரும்பு false;
}

முழு sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	எனில் (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		திரும்பு SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	எனில் (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} இல்லையெனில் {
		எனில் (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} இல்லையெனில் {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	திரும்பு SBI_OK;
}

முழு sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	எனில் (!prot_out || !addr_out || !log2len)
		திரும்பு SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	எனில் ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		எனில் (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} இல்லையெனில் {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} இல்லையெனில் {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	திரும்பு SBI_OK;
}
