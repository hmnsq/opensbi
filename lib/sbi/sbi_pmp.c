/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_pmp.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ctz(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ x)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret = 0;

	ਜੇ (x == 0)
		ਵਾਪਸ 8 * ਆਕਾਰ(x);

	ਜਦੋਂ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ਵਾਪਸ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ਜੇ (pmp->cfg & PMP_A)
		ਵਾਪਸ true;

	ਵਾਪਸ false;
}

ਪੂਰਨ sbi_pmp_encode(pmp_t *pmp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ prot, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ log2len)
{
	/* check parameters */
	ਜੇ (log2len > __riscv_xlen || log2len < PMP_ਖਿਸਕਾਓ)
		ਵਾਪਸ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_ਖਿਸਕਾਓ) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ਜੇ (log2len == PMP_ਖਿਸਕਾਓ) {
		pmp->addr = (addr >> PMP_ਖਿਸਕਾਓ);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ਨਹੀਂਤਾਂ {
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addrmask;
			addrmask = (1UL << (log2len - PMP_ਖਿਸਕਾਓ)) - 1;
			pmp->addr = ((addr >> PMP_ਖਿਸਕਾਓ) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ਵਾਪਸ SBI_OK;
}

ਪੂਰਨ sbi_pmp_decode(pmp_t *pmp, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *prot_ਬਾਹਰਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *addr_ਬਾਹਰਲਾ,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *log2len)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ prot;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ t1, addr, len;

	/* check parameters */
	ਜੇ (!prot_ਬਾਹਰਲਾ || !addr_ਬਾਹਰਲਾ || !log2len)
		ਵਾਪਸ SBI_EINVAL;
	*prot_ਬਾਹਰਲਾ = *addr_ਬਾਹਰਲਾ = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ਜੇ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ਜੇ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ਨਹੀਂਤਾਂ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_ਖਿਸਕਾਓ;
			len	= (t1 + PMP_ਖਿਸਕਾਓ + 1);
		}
	} ਨਹੀਂਤਾਂ {
		addr	= pmp->addr << PMP_ਖਿਸਕਾਓ;
		len	= PMP_ਖਿਸਕਾਓ;
	}

	/* return details */
	*prot_ਬਾਹਰਲਾ    = prot;
	*addr_ਬਾਹਰਲਾ    = addr;
	*log2len     = len;

	ਵਾਪਸ SBI_OK;
}
