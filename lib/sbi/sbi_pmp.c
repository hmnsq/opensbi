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

स्थिर अचिह्नित दीर्घ ctz(अचिह्नित दीर्घ x)
{
	अचिह्नित दीर्घ ret = 0;

	यदि (x == 0)
		प्रत्यागम 8 * परिमाण(x);

	यावत् (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	प्रत्यागम ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	यदि (pmp->cfg & PMP_A)
		प्रत्यागम true;

	प्रत्यागम false;
}

पूर्णाङ्क sbi_pmp_encode(pmp_t *pmp, अचिह्नित दीर्घ prot, अचिह्नित दीर्घ addr,
		    अचिह्नित दीर्घ log2len)
{
	/* check parameters */
	यदि (log2len > __riscv_xlen || log2len < PMP_सरणम्)
		प्रत्यागम SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_सरणम्) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	यदि (log2len == PMP_सरणम्) {
		pmp->addr = (addr >> PMP_सरणम्);
	} अन्यथा {
		यदि (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} अन्यथा {
			अचिह्नित दीर्घ addrmask;
			addrmask = (1UL << (log2len - PMP_सरणम्)) - 1;
			pmp->addr = ((addr >> PMP_सरणम्) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	प्रत्यागम SBI_OK;
}

पूर्णाङ्क sbi_pmp_decode(pmp_t *pmp, अचिह्नित दीर्घ *prot_बहिर्गमनम्, अचिह्नित दीर्घ *addr_बहिर्गमनम्,
		    अचिह्नित दीर्घ *log2len)
{
	अचिह्नित दीर्घ prot;
	अचिह्नित दीर्घ t1, addr, len;

	/* check parameters */
	यदि (!prot_बहिर्गमनम् || !addr_बहिर्गमनम् || !log2len)
		प्रत्यागम SBI_EINVAL;
	*prot_बहिर्गमनम् = *addr_बहिर्गमनम् = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	यदि ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		यदि (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} अन्यथा {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_सरणम्;
			len	= (t1 + PMP_सरणम् + 1);
		}
	} अन्यथा {
		addr	= pmp->addr << PMP_सरणम्;
		len	= PMP_सरणम्;
	}

	/* return details */
	*prot_बहिर्गमनम्    = prot;
	*addr_बहिर्गमनम्    = addr;
	*log2len     = len;

	प्रत्यागम SBI_OK;
}
