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

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi_utils/ipi/andes_plicsw.h>

काष्ठा plicsw_तथ्य plicsw;

अटल व्योम plicsw_ipi_send(u32 hart_सूचकांक)
{
	ulong pending_reg;
	u32 interrupt_id, word_सूचकांक, pending_bit;
	u32 target_hart = sbi_hartindex_to_hartid(hart_सूचकांक);

	अगर (plicsw.hart_गिनती <= target_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	interrupt_id = target_hart + 1;
	word_सूचकांक   = interrupt_id / 32;
	pending_bit  = interrupt_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_BASE + word_सूचकांक * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(BIT(pending_bit), (व्योम *)pending_reg);
}

अटल व्योम plicsw_ipi_clear(व्योम)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_BASE + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	अगर (plicsw.hart_गिनती <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((व्योम *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (व्योम *)reg);
}

अटल काष्ठा sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

पूर्णांक plicsw_cold_ipi_init(काष्ठा plicsw_तथ्य *plicsw)
{
	पूर्णांक rc;
	u32 interrupt_id, word_सूचकांक, enable_bit;
	ulong enable_reg, priority_reg;

	/* Setup source priority */
	क्रम (पूर्णांक i = 0; i < plicsw->hart_गिनती; i++) {
		priority_reg = plicsw->addr + PLICSW_PRIORITY_BASE + i * 4;
		writel(1, (व्योम *)priority_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	क्रम (पूर्णांक i = 0; i < plicsw->hart_गिनती; i++) {
		interrupt_id = i + 1;
		word_सूचकांक   = interrupt_id / 32;
		enable_bit   = interrupt_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_BASE +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_सूचकांक;
		writel(BIT(enable_bit), (व्योम *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_मूल_add_memrange(plicsw->addr, plicsw->आकार,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	अगर (rc)
		वापस rc;

	sbi_ipi_add_device(&plicsw_ipi);

	वापस 0;
}
