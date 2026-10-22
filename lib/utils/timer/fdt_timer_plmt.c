/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/timer/fdt_timer.h>
#అన్తర్భావ <sbi_utils/timer/andes_plmt.h>

బాహ్య నిర్మాణం plmt_దత్తాంశం plmt;

స్థిర పూర్ణాంకం fdt_plmt_cold_timer_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				    స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ plmt_పునాది;

	rc = fdt_parse_plmt_కణుపు(fdt, nodeoff, &plmt_పునాది, &plmt.కొలత,
				 &plmt.hart_లెక్క);
	ఒకవేళ (rc)
		తిరిగి rc;

	plmt.సమయం_val = (u64 *)plmt_పునాది;
	plmt.సమయం_cmp = (u64 *)(plmt_పునాది + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = plmt_cold_timer_init(&plmt);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక timer_plmt_సరిపోలిక[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_timer_plmt = {
	.సరిపోలిక_పట్టిక = timer_plmt_సరిపోలిక,
	.init = fdt_plmt_cold_timer_init,
};
