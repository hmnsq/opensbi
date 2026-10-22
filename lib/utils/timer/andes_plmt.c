/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/andes_plmt.h>

ਬਣਤਰ plmt_ਅੰਕੜੇ plmt;

ਸਥਿਰ u64 plmt_timer_ਮੁੱਲ(ਖਾਲੀ)
{
#ਜੇ __riscv_xlen == 64
	ਵਾਪਸ readq_relaxed(plmt.ਸਮਾਂ_val);
#ਨਹੀਂਤਾਂ
	u32 lo, hi;

	ਕਰੋ {
		hi = readl_relaxed((ਖਾਲੀ *)plmt.ਸਮਾਂ_val + 0x04);
		lo = readl_relaxed(plmt.ਸਮਾਂ_val);
	} ਜਦੋਂ (hi != readl_relaxed((ਖਾਲੀ *)plmt.ਸਮਾਂ_val + 0x04));

	ਵਾਪਸ ((u64)hi << 32) | (u64)lo;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ plmt_timer_event_stop(ਖਾਲੀ)
{
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();

	ਜੇ (plmt.hart_ਗਿਣਤੀ <= ਨਿਸ਼ਾਨਾ_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ਜੇ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]);
#ਨਹੀਂਤਾਂ
	writel_relaxed(-1UL, &plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]);
	writel_relaxed(-1UL, (ਖਾਲੀ *)(&plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]) + 0x04);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ plmt_timer_event_ਸ਼ੁਰੂ(u64 ਅਗਲਾ_event)
{
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();

	ਜੇ (plmt.hart_ਗਿਣਤੀ <= ਨਿਸ਼ਾਨਾ_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ਜੇ __riscv_xlen == 64
	writeq_relaxed(ਅਗਲਾ_event, &plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]);
#ਨਹੀਂਤਾਂ
	u32 ਨਕਾਬ = -1UL;

	writel_relaxed(ਅਗਲਾ_event & ਨਕਾਬ, &plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]);
	writel_relaxed(ਅਗਲਾ_event >> 32,
		       (ਖਾਲੀ *)(&plmt.ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart]) + 0x04);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ plmt_warm_timer_init(ਖਾਲੀ)
{
	ਜੇ (!plmt.ਸਮਾਂ_val)
		ਵਾਪਸ SBI_ENODEV;

	plmt_timer_event_stop();

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_ਮੁੱਲ	   = plmt_timer_ਮੁੱਲ,
	.timer_event_ਸ਼ੁਰੂ = plmt_timer_event_ਸ਼ੁਰੂ,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

ਪੂਰਨ plmt_cold_timer_init(ਬਣਤਰ plmt_ਅੰਕੜੇ *plmt)
{
	ਪੂਰਨ rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(
		(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)plmt->ਸਮਾਂ_val, plmt->ਮਾਪ,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_ਸੈੱਟ_device(&plmt_timer);

	ਵਾਪਸ 0;
}
