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

ساكن بنانشان ڊگهو ctz(بنانشان ڊگهو x)
{
	بنانشان ڊگهو ret = 0;

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

صحيح sbi_pmp_encode(pmp_t *pmp, بنانشان ڊگهو prot, بنانشان ڊگهو addr,
		    بنانشان ڊگهو log2len)
{
	/* check parameters */
	إذا (log2len > __riscv_xlen || log2len < PMP_سرڪايو)
		أرجع SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_سرڪايو) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	إذا (log2len == PMP_سرڪايو) {
		pmp->addr = (addr >> PMP_سرڪايو);
	} وإلا {
		إذا (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} وإلا {
			بنانشان ڊگهو addrmask;
			addrmask = (1UL << (log2len - PMP_سرڪايو)) - 1;
			pmp->addr = ((addr >> PMP_سرڪايو) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	أرجع SBI_OK;
}

صحيح sbi_pmp_decode(pmp_t *pmp, بنانشان ڊگهو *prot_ٻاهريون, بنانشان ڊگهو *addr_ٻاهريون,
		    بنانشان ڊگهو *log2len)
{
	بنانشان ڊگهو prot;
	بنانشان ڊگهو t1, addr, len;

	/* check parameters */
	إذا (!prot_ٻاهريون || !addr_ٻاهريون || !log2len)
		أرجع SBI_EINVAL;
	*prot_ٻاهريون = *addr_ٻاهريون = *log2len = 0;

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
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_سرڪايو;
			len	= (t1 + PMP_سرڪايو + 1);
		}
	} وإلا {
		addr	= pmp->addr << PMP_سرڪايو;
		len	= PMP_سرڪايو;
	}

	/* return details */
	*prot_ٻاهريون    = prot;
	*addr_ٻاهريون    = addr;
	*log2len     = len;

	أرجع SBI_OK;
}
