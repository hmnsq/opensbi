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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/ipi/andes_plicsw.h>

ਬਣਤਰ plicsw_ਅੰਕੜੇ plicsw;

ਸਥਿਰ ਖਾਲੀ plicsw_ipi_send(u32 hart_ਸੂਚਕਾਂਕ)
{
	ulong pending_reg;
	u32 ਰੁਕਾਵਟ_id, ਸ਼ਬਦ_ਸੂਚਕਾਂਕ, pending_ਦੋਆਧਾਰੀ_ਅੰਕ;
	u32 ਨਿਸ਼ਾਨਾ_hart = sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(hart_ਸੂਚਕਾਂਕ);

	ਜੇ (plicsw.hart_ਗਿਣਤੀ <= ਨਿਸ਼ਾਨਾ_hart)
		ebreak();

	/*
	 * We assign a single bit for each hart.
	 * Bit 0 is hardwired to 0, thus unavailable.
	 * Bit(X+1) indicates that IPI is sent to hartX.
	 */
	ਰੁਕਾਵਟ_id = ਨਿਸ਼ਾਨਾ_hart + 1;
	ਸ਼ਬਦ_ਸੂਚਕਾਂਕ   = ਰੁਕਾਵਟ_id / 32;
	pending_ਦੋਆਧਾਰੀ_ਅੰਕ  = ਰੁਕਾਵਟ_id % 32;
	pending_reg  = plicsw.addr + PLICSW_PENDING_ਅਧਾਰ + ਸ਼ਬਦ_ਸੂਚਕਾਂਕ * 4;

	/* Set target hart's mip.MSIP */
	writel_relaxed(ਦੋਆਧਾਰੀ_ਅੰਕ(pending_ਦੋਆਧਾਰੀ_ਅੰਕ), (ਖਾਲੀ *)pending_reg);
}

ਸਥਿਰ ਖਾਲੀ plicsw_ipi_clear(ਖਾਲੀ)
{
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();
	ulong reg = plicsw.addr + PLICSW_CONTEXT_ਅਧਾਰ + PLICSW_CONTEXT_CLAIM +
		    PLICSW_CONTEXT_STRIDE * ਨਿਸ਼ਾਨਾ_hart;

	ਜੇ (plicsw.hart_ਗਿਣਤੀ <= ਨਿਸ਼ਾਨਾ_hart)
		ebreak();

	/* Claim */
	u32 ਸਰੋਤ = readl((ਖਾਲੀ *)reg);

	/* A successful claim will clear mip.MSIP */

	/* Complete */
	writel(ਸਰੋਤ, (ਖਾਲੀ *)reg);
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_device plicsw_ipi = {
	.name      = "andes_plicsw",
	.rating    = 200,
	.ipi_send  = plicsw_ipi_send,
	.ipi_clear = plicsw_ipi_clear
};

ਪੂਰਨ plicsw_cold_ipi_init(ਬਣਤਰ plicsw_ਅੰਕੜੇ *plicsw)
{
	ਪੂਰਨ rc;
	u32 ਰੁਕਾਵਟ_id, ਸ਼ਬਦ_ਸੂਚਕਾਂਕ, enable_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ulong enable_reg, ਤਰਜੀਹ_reg;

	/* Setup source priority */
	ਲਈ (ਪੂਰਨ i = 0; i < plicsw->hart_ਗਿਣਤੀ; i++) {
		ਤਰਜੀਹ_reg = plicsw->addr + PLICSW_ਤਰਜੀਹ_ਅਧਾਰ + i * 4;
		writel(1, (ਖਾਲੀ *)ਤਰਜੀਹ_reg);
	}

	/*
	 * Setup enable for each hart, skip non-existent interrupt ID 0
	 * which is hardwired to 0.
	 */
	ਲਈ (ਪੂਰਨ i = 0; i < plicsw->hart_ਗਿਣਤੀ; i++) {
		ਰੁਕਾਵਟ_id = i + 1;
		ਸ਼ਬਦ_ਸੂਚਕਾਂਕ   = ਰੁਕਾਵਟ_id / 32;
		enable_ਦੋਆਧਾਰੀ_ਅੰਕ   = ਰੁਕਾਵਟ_id % 32;
		enable_reg   = plicsw->addr + PLICSW_ENABLE_ਅਧਾਰ +
			       PLICSW_ENABLE_STRIDE * i + 4 * ਸ਼ਬਦ_ਸੂਚਕਾਂਕ;
		writel(ਦੋਆਧਾਰੀ_ਅੰਕ(enable_ਦੋਆਧਾਰੀ_ਅੰਕ), (ਖਾਲੀ *)enable_reg);
	}

	/* Add PLICSW region to the root domain */
	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(plicsw->addr, plicsw->ਮਾਪ,
					  PLICSW_REGION_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_ipi_ਜੋੜੋ_device(&plicsw_ipi);

	ਵਾਪਸ 0;
}
