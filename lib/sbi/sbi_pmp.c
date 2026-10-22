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

ساكن بیعلامت دراز ctz(بیعلامت دراز x)
{
	بیعلامت دراز ret = 0;

	إذا (x == 0)
		أرجع 8 * حجم(x);

	طالما (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	أرجع ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	إذا (pmp->cfg & PMP_A)
		أرجع true;

	أرجع false;
}

صحيح sbi_pmp_encode(pmp_t *pmp, بیعلامت دراز prot, بیعلامت دراز addr,
		    بیعلامت دراز log2len)
{
	/* check parameters */
	إذا (log2len > __riscv_xlen || log2len < PMP_لغزش)
		أرجع SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_لغزش) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	إذا (log2len == PMP_لغزش) {
		pmp->addr = (addr >> PMP_لغزش);
	} وإلا {
		إذا (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} وإلا {
			بیعلامت دراز addrmask;
			addrmask = (1UL << (log2len - PMP_لغزش)) - 1;
			pmp->addr = ((addr >> PMP_لغزش) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	أرجع SBI_OK;
}

صحيح sbi_pmp_decode(pmp_t *pmp, بیعلامت دراز *prot_بیرون, بیعلامت دراز *addr_بیرون,
		    بیعلامت دراز *log2len)
{
	بیعلامت دراز prot;
	بیعلامت دراز t1, addr, len;

	/* check parameters */
	إذا (!prot_بیرون || !addr_بیرون || !log2len)
		أرجع SBI_EINVAL;
	*prot_بیرون = *addr_بیرون = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	إذا ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		إذا (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} وإلا {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_لغزش;
			len	= (t1 + PMP_لغزش + 1);
		}
	} وإلا {
		addr	= pmp->addr << PMP_لغزش;
		len	= PMP_لغزش;
	}

	/* return details */
	*prot_بیرون    = prot;
	*addr_بیرون    = addr;
	*log2len     = len;

	أرجع SBI_OK;
}
