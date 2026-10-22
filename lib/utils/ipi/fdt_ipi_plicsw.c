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

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/ipi/andes_plicsw.h>

బాహ్య నిర్మాణం plicsw_దత్తాంశం plicsw;

పూర్ణాంకం fdt_plicsw_cold_ipi_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			     స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;

	rc = fdt_parse_plicsw_కణుపు(fdt, nodeoff, &plicsw.addr, &plicsw.కొలత,
				   &plicsw.hart_లెక్క);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక ipi_plicsw_సరిపోలిక[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_ipi_plicsw = {
	.సరిపోలిక_పట్టిక = ipi_plicsw_సరిపోలిక,
	.init = fdt_plicsw_cold_ipi_init,
};
