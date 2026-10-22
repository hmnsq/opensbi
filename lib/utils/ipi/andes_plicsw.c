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

ಸಂರಚನಾ plicsw_data plicsw;

ಸ್ಥಿರ ಶೂನ್ಯ plicsw_ipi_send(u32 hart_index)
{
	ulong pending_reg;
	u32 interrupt_id, word_index, pending_bit;
	u32 target_hart = sbi_hartindex_to_hartid(hart_index);

	ಯದಿ (plicsw.hart_ಎಣಿಕೆ <= target_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	interrupt_id = target_hart + 1;
	word_index   = interrupt_id / 32;
	pending_bit  = interrupt_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_BASE + word_index * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(BIT(pending_bit), (ಶೂನ್ಯ *)pending_reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ plicsw_ipi_clear(ಶೂನ್ಯ)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_BASE + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	ಯದಿ (plicsw.hart_ಎಣಿಕೆ <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((ಶೂನ್ಯ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (ಶೂನ್ಯ *)reg);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

ಪೂರ್ಣಾಙ್ಕ plicsw_cold_ipi_init(ಸಂರಚನಾ plicsw_data *plicsw)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	u32 interrupt_id, word_index, enable_bit;
	ulong enable_reg, priority_reg;

	/* Setup source priority */
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < plicsw->hart_ಎಣಿಕೆ; i++) {
		priority_reg = plicsw->addr + PLICSW_PRIORITY_BASE + i * 4;
		writel(1, (ಶೂನ್ಯ *)priority_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < plicsw->hart_ಎಣಿಕೆ; i++) {
		interrupt_id = i + 1;
		word_index   = interrupt_id / 32;
		enable_bit   = interrupt_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_BASE +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_index;
		writel(BIT(enable_bit), (ಶೂನ್ಯ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ಬೇರು_add_memrange(plicsw->addr, plicsw->size,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sbi_ipi_add_device(&plicsw_ipi);

	ಪ್ರತ್ಯಾಗಮ 0;
}
