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

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi_utils/ipi/andes_plicsw.h>

ഘടന plicsw_data plicsw;

സ്ഥിതം ശൂന്യം plicsw_ipi_send(u32 hart_index)
{
	ulong pending_reg;
	u32 interrupt_id, word_index, pending_bit;
	u32 target_hart = sbi_hartindex_to_hartid(hart_index);

	എങ്കിൽ (plicsw.hart_എണ്ണം <= target_hart)
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
	writel_relaxed(BIT(pending_bit), (ശൂന്യം *)pending_reg);
}

സ്ഥിതം ശൂന്യം plicsw_ipi_clear(ശൂന്യം)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_BASE + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	എങ്കിൽ (plicsw.hart_എണ്ണം <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((ശൂന്യം *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (ശൂന്യം *)reg);
}

സ്ഥിതം ഘടന sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

പൂർണ്ണസംഖ്യ plicsw_cold_ipi_init(ഘടന plicsw_data *plicsw)
{
	പൂർണ്ണസംഖ്യ rc;
	u32 interrupt_id, word_index, enable_bit;
	ulong enable_reg, priority_reg;

	/* Setup source priority */
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < plicsw->hart_എണ്ണം; i++) {
		priority_reg = plicsw->addr + PLICSW_PRIORITY_BASE + i * 4;
		writel(1, (ശൂന്യം *)priority_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < plicsw->hart_എണ്ണം; i++) {
		interrupt_id = i + 1;
		word_index   = interrupt_id / 32;
		enable_bit   = interrupt_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_BASE +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_index;
		writel(BIT(enable_bit), (ശൂന്യം *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_വേര്_add_memrange(plicsw->addr, plicsw->size,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sbi_ipi_add_device(&plicsw_ipi);

	തിരികെ 0;
}
