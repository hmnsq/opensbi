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

بنية plicsw_انکڑے plicsw;

ساكن فراغ plicsw_ipi_send(u32 hart_اشاریہ)
{
	ulong pending_reg;
	u32 رکاوٹ_id, لفظ_اشاریہ, pending_ثنائی_ہندسہ;
	u32 نشانہ_hart = sbi_hartindex_منزل_hartid(hart_اشاریہ);

	إذا (plicsw.hart_گنتی <= نشانہ_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	رکاوٹ_id = نشانہ_hart + 1;
	لفظ_اشاریہ   = رکاوٹ_id / 32;
	pending_ثنائی_ہندسہ  = رکاوٹ_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_ادھار + لفظ_اشاریہ * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(ثنائی_ہندسہ(pending_ثنائی_ہندسہ), (فراغ *)pending_reg);
}

ساكن فراغ plicsw_ipi_clear(فراغ)
{
	u32 نشانہ_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_ادھار + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * نشانہ_hart;

	إذا (plicsw.hart_گنتی <= نشانہ_hart)
		ebreak();

	/* Claim */
	u32 سروت = readl((فراغ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(سروت, (فراغ *)reg);
}

ساكن بنية sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

صحيح plicsw_cold_ipi_init(بنية plicsw_انکڑے *plicsw)
{
	صحيح rc;
	u32 رکاوٹ_id, لفظ_اشاریہ, enable_ثنائی_ہندسہ;
	ulong enable_reg, ترجیح_reg;

	/* Setup source priority */
	لكل (صحيح i = 0; i < plicsw->hart_گنتی; i++) {
		ترجیح_reg = plicsw->addr + PLICSW_ترجیح_ادھار + i * 4;
		writel(1, (فراغ *)ترجیح_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	لكل (صحيح i = 0; i < plicsw->hart_گنتی; i++) {
		رکاوٹ_id = i + 1;
		لفظ_اشاریہ   = رکاوٹ_id / 32;
		enable_ثنائی_ہندسہ   = رکاوٹ_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_ادھار +
			       PLICSW_ENABLE_STRIDE * i + 4 * لفظ_اشاریہ;
		writel(ثنائی_ہندسہ(enable_ثنائی_ہندسہ), (فراغ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_جڑ_جوڑو_memrange(plicsw->addr, plicsw->ناپ,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_جوڑو_device(&plicsw_ipi);

	أرجع 0;
}
