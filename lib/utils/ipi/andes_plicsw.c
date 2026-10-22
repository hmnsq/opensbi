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

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi_utils/ipi/andes_plicsw.h>

কাঠামো plicsw_তথ্য plicsw;

স্থির শূন্য plicsw_ipi_send(u32 hart_সূচক)
{
	ulong pending_reg;
	u32 interrupt_id, word_সূচক, pending_bit;
	u32 target_hart = sbi_hartindex_to_hartid(hart_সূচক);

	যদি (plicsw.hart_গুনতি <= target_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	interrupt_id = target_hart + 1;
	word_সূচক   = interrupt_id / 32;
	pending_bit  = interrupt_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_BASE + word_সূচক * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(BIT(pending_bit), (শূন্য *)pending_reg);
}

স্থির শূন্য plicsw_ipi_clear(শূন্য)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_BASE + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	যদি (plicsw.hart_গুনতি <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((শূন্য *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (শূন্য *)reg);
}

স্থির কাঠামো sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

পূর্ণসংখ্যা plicsw_cold_ipi_init(কাঠামো plicsw_তথ্য *plicsw)
{
	পূর্ণসংখ্যা rc;
	u32 interrupt_id, word_সূচক, enable_bit;
	ulong enable_reg, priority_reg;

	/* Setup source priority */
	জন্য (পূর্ণসংখ্যা i = 0; i < plicsw->hart_গুনতি; i++) {
		priority_reg = plicsw->addr + PLICSW_PRIORITY_BASE + i * 4;
		writel(1, (শূন্য *)priority_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	জন্য (পূর্ণসংখ্যা i = 0; i < plicsw->hart_গুনতি; i++) {
		interrupt_id = i + 1;
		word_সূচক   = interrupt_id / 32;
		enable_bit   = interrupt_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_BASE +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_সূচক;
		writel(BIT(enable_bit), (শূন্য *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_মূল_add_memrange(plicsw->addr, plicsw->আকার_2,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	যদি (rc)
		ফেরত rc;

	sbi_ipi_add_device(&plicsw_ipi);

	ফেরত 0;
}
