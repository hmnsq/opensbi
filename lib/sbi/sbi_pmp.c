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

स्थिर अचिन्हित दीर्घ ctz(अचिन्हित दीर्घ x)
{
	अचिन्हित दीर्घ ret = 0;

	जर (x == 0)
		परत 8 * आकार(x);

	पर्यंत (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	परत ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	जर (pmp->cfg & PMP_A)
		परत true;

	परत false;
}

पूर्णांक sbi_pmp_encode(pmp_t *pmp, अचिन्हित दीर्घ prot, अचिन्हित दीर्घ addr,
		    अचिन्हित दीर्घ log2len)
{
	/* check parameters */
	जर (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		परत SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	जर (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} अन्यथा {
		जर (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} अन्यथा {
			अचिन्हित दीर्घ addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	परत SBI_OK;
}

पूर्णांक sbi_pmp_decode(pmp_t *pmp, अचिन्हित दीर्घ *prot_out, अचिन्हित दीर्घ *addr_out,
		    अचिन्हित दीर्घ *log2len)
{
	अचिन्हित दीर्घ prot;
	अचिन्हित दीर्घ t1, addr, len;

	/* check parameters */
	जर (!prot_out || !addr_out || !log2len)
		परत SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	जर ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		जर (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} अन्यथा {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} अन्यथा {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	परत SBI_OK;
}
