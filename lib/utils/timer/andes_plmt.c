/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/timer/andes_plmt.h>

నిర్మాణం plmt_దత్తాంశం plmt;

స్థిర u64 plmt_timer_విలువ(శూన్య)
{
#ఒకవేళ __riscv_xlen == 64
	తిరిగి readq_relaxed(plmt.సమయం_val);
#లేకపోతే
	u32 lo, hi;

	చేయి {
		hi = readl_relaxed((శూన్య *)plmt.సమయం_val + 0x04);
		lo = readl_relaxed(plmt.సమయం_val);
	} అయితే (hi != readl_relaxed((శూన్య *)plmt.సమయం_val + 0x04));

	తిరిగి ((u64)hi << 32) | (u64)lo;
#అయితే_ముగింపు
}

స్థిర శూన్య plmt_timer_event_stop(శూన్య)
{
	u32 లక్ష్యం_hart = current_hartid();

	ఒకవేళ (plmt.hart_లెక్క <= లక్ష్యం_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#ఒకవేళ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.సమయం_cmp[లక్ష్యం_hart]);
#లేకపోతే
	writel_relaxed(-1UL, &plmt.సమయం_cmp[లక్ష్యం_hart]);
	writel_relaxed(-1UL, (శూన్య *)(&plmt.సమయం_cmp[లక్ష్యం_hart]) + 0x04);
#అయితే_ముగింపు
}

స్థిర శూన్య plmt_timer_event_ప్రారంభం(u64 తదుపరి_event)
{
	u32 లక్ష్యం_hart = current_hartid();

	ఒకవేళ (plmt.hart_లెక్క <= లక్ష్యం_hart)
		ebreak();

	/* Program PLMT Time Compare */
#ఒకవేళ __riscv_xlen == 64
	writeq_relaxed(తదుపరి_event, &plmt.సమయం_cmp[లక్ష్యం_hart]);
#లేకపోతే
	u32 ముసుగు = -1UL;

	writel_relaxed(తదుపరి_event & ముసుగు, &plmt.సమయం_cmp[లక్ష్యం_hart]);
	writel_relaxed(తదుపరి_event >> 32,
		       (శూన్య *)(&plmt.సమయం_cmp[లక్ష్యం_hart]) + 0x04);
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం plmt_warm_timer_init(శూన్య)
{
	ఒకవేళ (!plmt.సమయం_val)
		తిరిగి SBI_ENODEV;

	plmt_timer_event_stop();

	తిరిగి 0;
}

స్థిర నిర్మాణం sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_విలువ	   = plmt_timer_విలువ,
	.timer_event_ప్రారంభం = plmt_timer_event_ప్రారంభం,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

పూర్ణాంకం plmt_cold_timer_init(నిర్మాణం plmt_దత్తాంశం *plmt)
{
	పూర్ణాంకం rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_మూలం_కూడు_memrange(
		(గుర్తులేని దీర్ఘ)plmt->సమయం_val, plmt->కొలత,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ఒకవేళ (rc)
		తిరిగి rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_సమితి_device(&plmt_timer);

	తిరిగి 0;
}
