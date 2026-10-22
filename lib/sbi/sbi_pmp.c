/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_pmp.h>

ಸ್ಥಿರ unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	ಯದಿ (x == 0)
		ಪ್ರತ್ಯಾಗಮ 8 * ಪರಿಮಾಣ(x);

	ಯಾವತ್ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ಯದಿ (pmp->cfg & PMP_A)
		ಪ್ರತ್ಯಾಗಮ true;

	ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	ಯದಿ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ಯದಿ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} ಅನ್ಯಥಾ {
		ಯದಿ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ಅನ್ಯಥಾ {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಪೂರ್ಣಾಙ್ಕ sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	ಯದಿ (!prot_out || !addr_out || !log2len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ಯದಿ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ಯದಿ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ಅನ್ಯಥಾ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} ಅನ್ಯಥಾ {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}
