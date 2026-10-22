/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Zong Li <zong@andestech.com>
 *   Nylon Chen <nylon7@andestech.com>
 *   Leo Yu-Chi Liang <ycliang@andestech.com>
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/ipi/andes_plicsw.h>

ರಚನೆ plicsw_ದತ್ತಾಂಶ plicsw;

ಸ್ಥಿರ ಶೂನ್ಯ plicsw_ipi_send(u32 hart_ಸೂಚ್ಯಂಕ)
{
	ulong pending_reg;
	u32 ಅಡಚಣೆ_id, ಶಬ್ದ_ಸೂಚ್ಯಂಕ, pending_ಬಿಟ್;
	u32 ಗುರಿ_hart = sbi_hartindex_ಗಮ್ಯ_hartid(hart_ಸೂಚ್ಯಂಕ);

	ಆದರೆ (plicsw.hart_ಎಣಿಕೆ <= ಗುರಿ_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	ಅಡಚಣೆ_id = ಗುರಿ_hart + 1;
	ಶಬ್ದ_ಸೂಚ್ಯಂಕ   = ಅಡಚಣೆ_id / 32;
	pending_ಬಿಟ್  = ಅಡಚಣೆ_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_ತಳ + ಶಬ್ದ_ಸೂಚ್ಯಂಕ * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(ಬಿಟ್(pending_ಬಿಟ್), (ಶೂನ್ಯ *)pending_reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ plicsw_ipi_clear(ಶೂನ್ಯ)
{
	u32 ಗುರಿ_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_ತಳ + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * ಗುರಿ_hart;

	ಆದರೆ (plicsw.hart_ಎಣಿಕೆ <= ಗುರಿ_hart)
		ebreak();

	/* Claim */
	u32 ಮೂಲ = readl((ಶೂನ್ಯ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(ಮೂಲ, (ಶೂನ್ಯ *)reg);
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

ಪೂರ್ಣಾಂಕ plicsw_cold_ipi_init(ರಚನೆ plicsw_ದತ್ತಾಂಶ *plicsw)
{
	ಪೂರ್ಣಾಂಕ rc;
	u32 ಅಡಚಣೆ_id, ಶಬ್ದ_ಸೂಚ್ಯಂಕ, enable_ಬಿಟ್;
	ulong enable_reg, ಆದ್ಯತೆ_reg;

	/* Setup source priority */
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < plicsw->hart_ಎಣಿಕೆ; i++) {
		ಆದ್ಯತೆ_reg = plicsw->addr + PLICSW_ಆದ್ಯತೆ_ತಳ + i * 4;
		writel(1, (ಶೂನ್ಯ *)ಆದ್ಯತೆ_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < plicsw->hart_ಎಣಿಕೆ; i++) {
		ಅಡಚಣೆ_id = i + 1;
		ಶಬ್ದ_ಸೂಚ್ಯಂಕ   = ಅಡಚಣೆ_id / 32;
		enable_ಬಿಟ್   = ಅಡಚಣೆ_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_ತಳ +
			       PLICSW_ENABLE_STRIDE * i + 4 * ಶಬ್ದ_ಸೂಚ್ಯಂಕ;
		writel(ಬಿಟ್(enable_ಬಿಟ್), (ಶೂನ್ಯ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(plicsw->addr, plicsw->ಗಾತ್ರ_2,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_ipi_ಕೂಡಿಸು_device(&plicsw_ipi);

	ಹಿಂತಿರುಗಿ 0;
}
