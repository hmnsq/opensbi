/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/serial/fdt_serial.h>
#అన్తర్భావ <sbi_utils/sys/htif.h>

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక serial_htif_సరిపోలిక[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

స్థిర పూర్ణాంకం serial_htif_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			    స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ఒకవేళ (!fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &fromhost_addr, ఖాళీ)) {
		custom = true;
		tohost_addr = fromhost_addr + పరిమాణం(uint64_t);
	}

	fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 1, &tohost_addr, ఖాళీ);

	rc = sbi_domain_మూలం_కూడు_memrange(fromhost_addr, పుట_కొలత, పుట_కొలత,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ఒకవేళ (rc)
		తిరిగి rc;

	తిరిగి htif_serial_init(custom, fromhost_addr, tohost_addr);
}

స్థిరరాశి నిర్మాణం fdt_driver fdt_serial_htif = {
	.సరిపోలిక_పట్టిక = serial_htif_సరిపోలిక,
	.init = serial_htif_init
};
