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

ایستا unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	اگر (x == 0)
		بازگشت 8 * اندازه(x);

	وقتی (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	بازگشت ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	اگر (pmp->cfg & PMP_A)
		بازگشت true;

	بازگشت false;
}

صحیح sbi_pmp_encode(pmp_t *pmp, unsigned long prot, unsigned long addr,
		    unsigned long log2len)
{
	/* check parameters */
	اگر (log2len > __riscv_xlen || log2len < PMP_SHIFT)
		بازگشت SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	اگر (log2len == PMP_SHIFT) {
		pmp->addr = (addr >> PMP_SHIFT);
	} وگرنه {
		اگر (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} وگرنه {
			unsigned long addrmask;
			addrmask = (1UL << (log2len - PMP_SHIFT)) - 1;
			pmp->addr = ((addr >> PMP_SHIFT) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	بازگشت SBI_OK;
}

صحیح sbi_pmp_decode(pmp_t *pmp, unsigned long *prot_out, unsigned long *addr_out,
		    unsigned long *log2len)
{
	unsigned long prot;
	unsigned long t1, addr, len;

	/* check parameters */
	اگر (!prot_out || !addr_out || !log2len)
		بازگشت SBI_EINVAL;
	*prot_out = *addr_out = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	اگر ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		اگر (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} وگرنه {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			len	= (t1 + PMP_SHIFT + 1);
		}
	} وگرنه {
		addr	= pmp->addr << PMP_SHIFT;
		len	= PMP_SHIFT;
	}

	/* return details */
	*prot_out    = prot;
	*addr_out    = addr;
	*log2len     = len;

	بازگشت SBI_OK;
}
