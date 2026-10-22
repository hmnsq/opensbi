/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_encoding.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_pmp.h>

स्थिर unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	जइ (x == 0)
		प्रत्यागम 8 * परिमाण(x);

	जाव (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	प्रत्यागम ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	जइ (pmp->cfg & PMP_A)
		प्रत्यागम true;

	प्रत्यागम false;
}

पूर्णाङ्क sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	जइ (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		प्रत्यागम SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	जइ (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} अण्णहा {
		जइ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} अण्णहा {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	जइ (!prot_out || !addr_out || !log2len)
		प्रत्यागम SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	जइ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		जइ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} अण्णहा {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} अण्णहा {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	प्रत्यागम SBI_OK;
}
