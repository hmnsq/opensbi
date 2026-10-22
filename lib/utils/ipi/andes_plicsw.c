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

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/ipi/andes_plicsw.h>

ସଂରଚନା plicsw_data plicsw;

ସ୍ଥିର ଶୂନ୍ୟ plicsw_ipi_send(u32 hart_index)
{
	ulong pending_reg;
	u32 interrupt_id, word_index, pending_bit;
	u32 target_hart = sbi_hartindex_to_hartid(hart_index);

	ଯଦି (plicsw.hart_ଗଣନା <= target_hart)
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
	writel_relaxed(BIT(pending_bit), (ଶୂନ୍ୟ *)pending_reg);
}

ସ୍ଥିର ଶୂନ୍ୟ plicsw_ipi_clear(ଶୂନ୍ୟ)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_BASE + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	ଯଦି (plicsw.hart_ଗଣନା <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((ଶୂନ୍ୟ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (ଶୂନ୍ୟ *)reg);
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା plicsw_cold_ipi_init(ସଂରଚନା plicsw_data *plicsw)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	u32 interrupt_id, word_index, enable_bit;
	ulong enable_reg, priority_reg;

	/* Setup source priority */
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < plicsw->hart_ଗଣନା; i++) {
		priority_reg = plicsw->addr + PLICSW_PRIORITY_BASE + i * 4;
		writel(1, (ଶୂନ୍ୟ *)priority_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < plicsw->hart_ଗଣନା; i++) {
		interrupt_id = i + 1;
		word_index   = interrupt_id / 32;
		enable_bit   = interrupt_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_BASE +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_index;
		writel(BIT(enable_bit), (ଶୂନ୍ୟ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ମୂଳ_add_memrange(plicsw->addr, plicsw->size,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sbi_ipi_add_device(&plicsw_ipi);

	ଫେରନ୍ତୁ 0;
}
