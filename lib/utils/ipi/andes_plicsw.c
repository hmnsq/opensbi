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

ساكن فراغ plicsw_ipi_send(u32 hart_شاخص)
{
	ulong pending_reg;
	u32 مداخله_id, کلمه_شاخص, pending_بټ;
	u32 موخه_hart = sbi_hartindex_منزل_hartid(hart_شاخص);

	إذا (plicsw.hart_شمېر <= موخه_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	مداخله_id = موخه_hart + 1;
	کلمه_شاخص   = مداخله_id / 32;
	pending_بټ  = مداخله_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_بنسټ + کلمه_شاخص * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(بټ(pending_بټ), (فراغ *)pending_reg);
}

ساكن فراغ plicsw_ipi_clear(فراغ)
{
	u32 موخه_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_بنسټ + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * موخه_hart;

	إذا (plicsw.hart_شمېر <= موخه_hart)
		ebreak();

	/* Claim */
	u32 سرچینه = readl((فراغ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(سرچینه, (فراغ *)reg);
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
	u32 مداخله_id, کلمه_شاخص, enable_بټ;
	ulong enable_reg, لومړیتوب_reg;

	/* Setup source priority */
	لكل (صحيح i = 0; i < plicsw->hart_شمېر; i++) {
		لومړیتوب_reg = plicsw->addr + PLICSW_لومړیتوب_بنسټ + i * 4;
		writel(1, (فراغ *)لومړیتوب_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	لكل (صحيح i = 0; i < plicsw->hart_شمېر; i++) {
		مداخله_id = i + 1;
		کلمه_شاخص   = مداخله_id / 32;
		enable_بټ   = مداخله_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_بنسټ +
			       PLICSW_ENABLE_STRIDE * i + 4 * کلمه_شاخص;
		writel(بټ(enable_بټ), (فراغ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ریښه_جمع_کړه_memrange(plicsw->addr, plicsw->اندازه,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_جمع_کړه_device(&plicsw_ipi);

	أرجع 0;
}
