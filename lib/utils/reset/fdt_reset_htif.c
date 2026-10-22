/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/sys/htif.h>

స్థిర పూర్ణాంకం htif_reset_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ఒకవేళ (!fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &fromhost_addr, ఖాళీ)) {
		custom = true;
		tohost_addr = fromhost_addr + పరిమాణం(uint64_t);
	}

	fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 1, &tohost_addr, ఖాళీ);

	తిరిగి htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక htif_reset_సరిపోలిక[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_reset_htif = {
	.సరిపోలిక_పట్టిక = htif_reset_సరిపోలిక,
	.init = htif_reset_init
};
