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

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ctz(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ x)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret = 0;

	ಆದರೆ (x == 0)
		ಹಿಂತಿರುಗಿ 8 * ಗಾತ್ರ(x);

	ಆಗ (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}

	ಹಿಂತಿರುಗಿ ret;
}

bool sbi_pmp_is_enabled(pmp_t *pmp)
{
	/* If address matching bits are non-zero, the entry is enable */
	ಆದರೆ (pmp->cfg & PMP_A)
		ಹಿಂತಿರುಗಿ true;

	ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ sbi_pmp_encode(pmp_t *pmp, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ prot, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr,
		    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ log2len)
{
	/* check parameters */
	ಆದರೆ (log2len > __riscv_xlen || log2len < PMP_ಸರಿಸು)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* encode PMP config */
	prot &= ~PMP_A;
	prot |= (log2len == PMP_ಸರಿಸು) ? PMP_A_NA4 : PMP_A_NAPOT;
	pmp->cfg = prot;

	/* encode PMP address */
	ಆದರೆ (log2len == PMP_ಸರಿಸು) {
		pmp->addr = (addr >> PMP_ಸರಿಸು);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (log2len == __riscv_xlen) {
			pmp->addr = -1UL;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addrmask;
			addrmask = (1UL << (log2len - PMP_ಸರಿಸು)) - 1;
			pmp->addr = ((addr >> PMP_ಸರಿಸು) & ~addrmask);
			pmp->addr |= (addrmask >> 1);
		}
	}

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಪೂರ್ಣಾಂಕ sbi_pmp_decode(pmp_t *pmp, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *prot_ಹೊರಹೋಗು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *addr_ಹೊರಹೋಗು,
		    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *log2len)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ prot;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ t1, addr, len;

	/* check parameters */
	ಆದರೆ (!prot_ಹೊರಹೋಗು || !addr_ಹೊರಹೋಗು || !log2len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	*prot_ಹೊರಹೋಗು = *addr_ಹೊರಹೋಗು = *log2len = 0;

	/* decode PMP config */
	prot = pmp->cfg;

	/* decode PMP address */
	ಆದರೆ ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = pmp->addr;
		ಆದರೆ (addr == -1UL) {
			addr	= 0;
			len	= __riscv_xlen;
		} ಇಲ್ಲದಿದ್ದರೆ {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_ಸರಿಸು;
			len	= (t1 + PMP_ಸರಿಸು + 1);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		addr	= pmp->addr << PMP_ಸರಿಸು;
		len	= PMP_ಸರಿಸು;
	}

	/* return details */
	*prot_ಹೊರಹೋಗು    = prot;
	*addr_ಹೊರಹೋಗು    = addr;
	*log2len     = len;

	ಹಿಂತಿರುಗಿ SBI_OK;
}
