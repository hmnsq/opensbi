/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

கட்டமைப்பு reset_metanoia_data {
	u8 *reg_base;
};

நிலையான கட்டமைப்பு reset_metanoia_data reset_data;

நிலையான முழு metanoia_system_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
		திரும்பு 1;
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
	இயல்பு:
		திரும்பு 0;
	}
}

நிலையான வெறுமை metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

நிலையான கட்டமைப்பு sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

நிலையான முழு metanoia_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			       மாறிலி கட்டமைப்பு fdt_match *match)
{
	u64 reg_addr;
	முழு rc;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, வெற்று);
	எனில் (rc < 0 || !reg_addr)
		திரும்பு SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
