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

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi_utils/ipi/andes_plicsw.h>

संरचना plicsw_दत्त plicsw;

स्थिर शून्य plicsw_ipi_send(u32 hart_सूचकांक)
{
	ulong pending_reg;
	u32 व्यवधानम्_id, word_सूचकांक, pending_bit;
	u32 target_hart = sbi_hartindex_गन्तव्यम्_hartid(hart_सूचकांक);

	यदि (plicsw.hart_गणनम् <= target_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	व्यवधानम्_id = target_hart + 1;
	word_सूचकांक   = व्यवधानम्_id / 32;
	pending_bit  = व्यवधानम्_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_आधारः + word_सूचकांक * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(BIT(pending_bit), (शून्य *)pending_reg);
}

स्थिर शून्य plicsw_ipi_clear(शून्य)
{
	u32 target_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_आधारः + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * target_hart;

	यदि (plicsw.hart_गणनम् <= target_hart)
		ebreak();

	/* Claim */
	u32 source = readl((शून्य *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(source, (शून्य *)reg);
}

स्थिर संरचना sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

पूर्णाङ्क plicsw_cold_ipi_init(संरचना plicsw_दत्त *plicsw)
{
	पूर्णाङ्क rc;
	u32 व्यवधानम्_id, word_सूचकांक, enable_bit;
	ulong enable_reg, प्राथमिकता_reg;

	/* Setup source priority */
	प्रति (पूर्णाङ्क i = 0; i < plicsw->hart_गणनम्; i++) {
		प्राथमिकता_reg = plicsw->addr + PLICSW_प्राथमिकता_आधारः + i * 4;
		writel(1, (शून्य *)प्राथमिकता_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	प्रति (पूर्णाङ्क i = 0; i < plicsw->hart_गणनम्; i++) {
		व्यवधानम्_id = i + 1;
		word_सूचकांक   = व्यवधानम्_id / 32;
		enable_bit   = व्यवधानम्_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_आधारः +
			       PLICSW_ENABLE_STRIDE * i + 4 * word_सूचकांक;
		writel(BIT(enable_bit), (शून्य *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_मूलम्_सङ्कलय_memrange(plicsw->addr, plicsw->आकार,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	यदि (rc)
		प्रत्यागम rc;

	sbi_ipi_सङ्कलय_device(&plicsw_ipi);

	प्रत्यागम 0;
}
