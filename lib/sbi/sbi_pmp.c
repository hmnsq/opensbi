/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_encoding.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_pmp.h>

ساکن غیرعلامتی طویل ctz(غیرعلامتی طویل x)
{
	غیرعلامتی طویل ret = 0;

	اگر (x == 0)
		واپس 8 * حجم(x);

	جب (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	واپس ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	اگر (pmp->cfg & PMP_A)
		واپس true;

	واپس false;
}

صحیح sbi_pmp_encode(pmp_t *pmp, غیرعلامتی طویل prot, غیرعلامتی طویل addr,
		    غیرعلامتی طویل log2len)
{
	/* check parameters */
	اگر (log2len > __riscv_xlen || log2len < PMP_سرکاؤ)
		واپس SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_سرکاؤ) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	اگر (log2len == PMP_سرکاؤ) {
		pmp->addr = (addr >> PMP_سرکاؤ);
	} ورنہ {
		اگر (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ورنہ {
			غیرعلامتی طویل addrmask;
			addrmask = (1UL << (log2len - PMP_سرکاؤ)) - 1;
			pmp->addr = ((addr >> PMP_سرکاؤ) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	واپس SBI_OK;
}

صحیح sbi_pmp_decode(pmp_t *pmp, غیرعلامتی طویل *prot_برآمد, غیرعلامتی طویل *addr_برآمد,
		    غیرعلامتی طویل *log2len)
{
	غیرعلامتی طویل prot;
	غیرعلامتی طویل t1, addr, len;

	/* check parameters */
	اگر (!prot_برآمد || !addr_برآمد || !log2len)
		واپس SBI_EINVAL;
	*prot_برآمد = *addr_برآمد = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	اگر ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		اگر (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ورنہ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_سرکاؤ;
			len	= (t1 + PMP_سرکاؤ + 1);
		}
	} ورنہ {
		addr	= pmp->addr << PMP_سرکاؤ;
		len	= PMP_سرکاؤ;
	}

	/* return details */
	*prot_برآمد    = prot;
	*addr_برآمد    = addr;
	*log2len     = len;

	واپس SBI_OK;
}
