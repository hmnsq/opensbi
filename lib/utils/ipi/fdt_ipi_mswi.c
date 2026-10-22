/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/ipi/aclint_mswi.h>

స్థిర పూర్ణాంకం ipi_mswi_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	గుర్తులేని దీర్ఘ స్థానభ్రంశం;
	నిర్మాణం aclint_mswi_దత్తాంశం *ms;

	ms = sbi_zalloc(పరిమాణం(*ms));
	ఒకవేళ (!ms)
		తిరిగి SBI_ENOMEM;

	rc = fdt_parse_aclint_కణుపు(fdt, nodeoff, false, false,
				   &ms->addr, &ms->కొలత, ఖాళీ, ఖాళీ,
				   &ms->మొదటి_hartid, &ms->hart_లెక్క);
	ఒకవేళ (rc) {
		sbi_free(ms);
		తిరిగి rc;
	}

	ఒకవేళ (సరిపోలిక->దత్తాంశం) {
		/* Adjust MSWI address and size for CLINT device */
		స్థానభ్రంశం = *((గుర్తులేని దీర్ఘ *)సరిపోలిక->దత్తాంశం);
		ms->addr += స్థానభ్రంశం;
		ఒకవేళ ((ms->కొలత - స్థానభ్రంశం) < ACLINT_MSWI_కొలత)
			తిరిగి SBI_EINVAL;
		ms->కొలత = ACLINT_MSWI_కొలత;
	}

	rc = aclint_mswi_cold_init(ms);
	ఒకవేళ (rc) {
		sbi_free(ms);
		తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ clint_స్థానభ్రంశం = CLINT_MSWI_స్థానభ్రంశం;

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక ipi_mswi_సరిపోలిక[] = {
	{ .compatible = "riscv,clint0", .దత్తాంశం = &clint_స్థానభ్రంశం },
	{ .compatible = "sifive,clint0", .దత్తాంశం = &clint_స్థానభ్రంశం },
	{ .compatible = "thead,c900-clint", .దత్తాంశం = &clint_స్థానభ్రంశం },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_ipi_mswi = {
	.సరిపోలిక_పట్టిక = ipi_mswi_సరిపోలిక,
	.init = ipi_mswi_cold_init,
};
