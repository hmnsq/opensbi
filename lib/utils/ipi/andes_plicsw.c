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

بنية plicsw_داده plicsw;

ساكن فراغ plicsw_ipi_send(u32 hart_شاخص)
{
	ulong pending_reg;
	u32 وقفه_id, کلمه_شاخص, pending_بیت;
	u32 هدف_hart = sbi_hartindex_مقصد_hartid(hart_شاخص);

	إذا (plicsw.hart_شمار <= هدف_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	وقفه_id = هدف_hart + 1;
	کلمه_شاخص   = وقفه_id / 32;
	pending_بیت  = وقفه_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_پایه + کلمه_شاخص * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(بیت(pending_بیت), (فراغ *)pending_reg);
}

ساكن فراغ plicsw_ipi_clear(فراغ)
{
	u32 هدف_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_پایه + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * هدف_hart;

	إذا (plicsw.hart_شمار <= هدف_hart)
		ebreak();

	/* Claim */
	u32 منبع = readl((فراغ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(منبع, (فراغ *)reg);
}

ساكن بنية sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

صحيح plicsw_cold_ipi_init(بنية plicsw_داده *plicsw)
{
	صحيح rc;
	u32 وقفه_id, کلمه_شاخص, enable_بیت;
	ulong enable_reg, اولویت_reg;

	/* Setup source priority */
	لكل (صحيح i = 0; i < plicsw->hart_شمار; i++) {
		اولویت_reg = plicsw->addr + PLICSW_اولویت_پایه + i * 4;
		writel(1, (فراغ *)اولویت_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	لكل (صحيح i = 0; i < plicsw->hart_شمار; i++) {
		وقفه_id = i + 1;
		کلمه_شاخص   = وقفه_id / 32;
		enable_بیت   = وقفه_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_پایه +
			       PLICSW_ENABLE_STRIDE * i + 4 * کلمه_شاخص;
		writel(بیت(enable_بیت), (فراغ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ریشه_جمع_کن_memrange(plicsw->addr, plicsw->اندازه,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_جمع_کن_device(&plicsw_ipi);

	أرجع 0;
}
