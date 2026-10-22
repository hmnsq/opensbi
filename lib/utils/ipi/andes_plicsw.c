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

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi_utils/ipi/andes_plicsw.h>

ساخت plicsw_ڈیٹا plicsw;

ساکن خالی plicsw_ipi_send(u32 hart_اشاریہ)
{
	ulong pending_reg;
	u32 مداخلت_id, لفظ_اشاریہ, pending_ثنائی_ہندسہ;
	u32 ہدف_hart = sbi_hartindex_مقصد_hartid(hart_اشاریہ);

	اگر (plicsw.hart_گنتی <= ہدف_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	مداخلت_id = ہدف_hart + 1;
	لفظ_اشاریہ   = مداخلت_id / 32;
	pending_ثنائی_ہندسہ  = مداخلت_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_بنیاد + لفظ_اشاریہ * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(ثنائی_ہندسہ(pending_ثنائی_ہندسہ), (خالی *)pending_reg);
}

ساکن خالی plicsw_ipi_clear(خالی)
{
	u32 ہدف_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_بنیاد + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * ہدف_hart;

	اگر (plicsw.hart_گنتی <= ہدف_hart)
		ebreak();

	/* Claim */
	u32 ماخذ = readl((خالی *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(ماخذ, (خالی *)reg);
}

ساکن ساخت sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

صحیح plicsw_cold_ipi_init(ساخت plicsw_ڈیٹا *plicsw)
{
	صحیح rc;
	u32 مداخلت_id, لفظ_اشاریہ, enable_ثنائی_ہندسہ;
	ulong enable_reg, ترجیح_reg;

	/* Setup source priority */
	برائے (صحیح i = 0; i < plicsw->hart_گنتی; i++) {
		ترجیح_reg = plicsw->addr + PLICSW_ترجیح_بنیاد + i * 4;
		writel(1, (خالی *)ترجیح_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	برائے (صحیح i = 0; i < plicsw->hart_گنتی; i++) {
		مداخلت_id = i + 1;
		لفظ_اشاریہ   = مداخلت_id / 32;
		enable_ثنائی_ہندسہ   = مداخلت_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_بنیاد +
			       PLICSW_ENABLE_STRIDE * i + 4 * لفظ_اشاریہ;
		writel(ثنائی_ہندسہ(enable_ثنائی_ہندسہ), (خالی *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_جڑ_جوڑو_memrange(plicsw->addr, plicsw->سائز,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	اگر (rc)
		واپس rc;

	sbi_ipi_جوڑو_device(&plicsw_ipi);

	واپس 0;
}
