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

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi_utils/ipi/andes_plicsw.h>

నిర్మాణం plicsw_దత్తాంశం plicsw;

స్థిర శూన్య plicsw_ipi_send(u32 hart_సూచీ)
{
	ulong pending_reg;
	u32 అంతరాయం_id, మాట_సూచీ, pending_బిట్టు;
	u32 లక్ష్యం_hart = sbi_hartindex_గమ్యం_hartid(hart_సూచీ);

	ఒకవేళ (plicsw.hart_లెక్క <= లక్ష్యం_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	అంతరాయం_id = లక్ష్యం_hart + 1;
	మాట_సూచీ   = అంతరాయం_id / 32;
	pending_బిట్టు  = అంతరాయం_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_పునాది + మాట_సూచీ * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(బిట్టు(pending_బిట్టు), (శూన్య *)pending_reg);
}

స్థిర శూన్య plicsw_ipi_clear(శూన్య)
{
	u32 లక్ష్యం_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_పునాది + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * లక్ష్యం_hart;

	ఒకవేళ (plicsw.hart_లెక్క <= లక్ష్యం_hart)
		ebreak();

	/* Claim */
	u32 ఆధారం = readl((శూన్య *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(ఆధారం, (శూన్య *)reg);
}

స్థిర నిర్మాణం sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

పూర్ణాంకం plicsw_cold_ipi_init(నిర్మాణం plicsw_దత్తాంశం *plicsw)
{
	పూర్ణాంకం rc;
	u32 అంతరాయం_id, మాట_సూచీ, enable_బిట్టు;
	ulong enable_reg, ప్రాధాన్యత_reg;

	/* Setup source priority */
	కొరకు (పూర్ణాంకం i = 0; i < plicsw->hart_లెక్క; i++) {
		ప్రాధాన్యత_reg = plicsw->addr + PLICSW_ప్రాధాన్యత_పునాది + i * 4;
		writel(1, (శూన్య *)ప్రాధాన్యత_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	కొరకు (పూర్ణాంకం i = 0; i < plicsw->hart_లెక్క; i++) {
		అంతరాయం_id = i + 1;
		మాట_సూచీ   = అంతరాయం_id / 32;
		enable_బిట్టు   = అంతరాయం_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_పునాది +
			       PLICSW_ENABLE_STRIDE * i + 4 * మాట_సూచీ;
		writel(బిట్టు(enable_బిట్టు), (శూన్య *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_మూలం_కూడు_memrange(plicsw->addr, plicsw->కొలత,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_ipi_కూడు_device(&plicsw_ipi);

	తిరిగి 0;
}
