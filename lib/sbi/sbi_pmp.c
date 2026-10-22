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

ساكن بینښه اوږد ctz(بینښه اوږد x)
{
	بینښه اوږد ret = 0;

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

صحيح sbi_pmp_encode(pmp_t *pmp, بینښه اوږد prot, بینښه اوږد addr,
		    بینښه اوږد log2len)
{
	/* check parameters */
	إذا (log2len > __riscv_xlen || log2len < PMP_ښويدنه)
		أرجع SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_ښويدنه) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	إذا (log2len == PMP_ښويدنه) {
		pmp->addr = (addr >> PMP_ښويدنه);
	} وإلا {
		إذا (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} وإلا {
			بینښه اوږد addrmask;
			addrmask = (1UL << (log2len - PMP_ښويدنه)) - 1;
			pmp->addr = ((addr >> PMP_ښويدنه) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	أرجع SBI_OK;
}

صحيح sbi_pmp_decode(pmp_t *pmp, بینښه اوږد *prot_بهر, بینښه اوږد *addr_بهر,
		    بینښه اوږد *log2len)
{
	بینښه اوږد prot;
	بینښه اوږد t1, addr, len;

	/* check parameters */
	إذا (!prot_بهر || !addr_بهر || !log2len)
		أرجع SBI_EINVAL;
	*prot_بهر = *addr_بهر = *log2len = 0;

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
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_ښويدنه;
			len	= (t1 + PMP_ښويدنه + 1);
		}
	} وإلا {
		addr	= pmp->addr << PMP_ښويدنه;
		len	= PMP_ښويدنه;
	}

	/* return details */
	*prot_بهر    = prot;
	*addr_بهر    = addr;
	*log2len     = len;

	أرجع SBI_OK;
}
