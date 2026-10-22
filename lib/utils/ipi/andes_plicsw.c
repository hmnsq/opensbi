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

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi_utils/ipi/andes_plicsw.h>

بنية plicsw_معلومات plicsw;

ساكن فراغ plicsw_ipi_send(u32 hart_اشاريو)
{
	ulong pending_reg;
	u32 مداخلت_id, لفظ_اشاريو, pending_ٻٽو_انگ;
	u32 هدف_hart = sbi_hartindex_منزل_hartid(hart_اشاريو);

	إذا (plicsw.hart_ڳڻپ <= هدف_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	مداخلت_id = هدف_hart + 1;
	لفظ_اشاريو   = مداخلت_id / 32;
	pending_ٻٽو_انگ  = مداخلت_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_بنياد + لفظ_اشاريو * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(ٻٽو_انگ(pending_ٻٽو_انگ), (فراغ *)pending_reg);
}

ساكن فراغ plicsw_ipi_clear(فراغ)
{
	u32 هدف_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_بنياد + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * هدف_hart;

	إذا (plicsw.hart_ڳڻپ <= هدف_hart)
		ebreak();

	/* Claim */
	u32 ذريعو = readl((فراغ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(ذريعو, (فراغ *)reg);
}

ساكن بنية sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

صحيح plicsw_cold_ipi_init(بنية plicsw_معلومات *plicsw)
{
	صحيح rc;
	u32 مداخلت_id, لفظ_اشاريو, enable_ٻٽو_انگ;
	ulong enable_reg, ترجيح_reg;

	/* Setup source priority */
	لكل (صحيح i = 0; i < plicsw->hart_ڳڻپ; i++) {
		ترجيح_reg = plicsw->addr + PLICSW_ترجيح_بنياد + i * 4;
		writel(1, (فراغ *)ترجيح_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	لكل (صحيح i = 0; i < plicsw->hart_ڳڻپ; i++) {
		مداخلت_id = i + 1;
		لفظ_اشاريو   = مداخلت_id / 32;
		enable_ٻٽو_انگ   = مداخلت_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_بنياد +
			       PLICSW_ENABLE_STRIDE * i + 4 * لفظ_اشاريو;
		writel(ٻٽو_انگ(enable_ٻٽو_انگ), (فراغ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_پاڙ_جوڙيو_memrange(plicsw->addr, plicsw->سائيز,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_جوڙيو_device(&plicsw_ipi);

	أرجع 0;
}
