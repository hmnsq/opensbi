/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

ഘടന reset_metanoia_data {
	u8 *reg_base;
};

സ്ഥിതം ഘടന reset_metanoia_data reset_data;

സ്ഥിതം പൂർണ്ണസംഖ്യ metanoia_system_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		തിരികെ 1;
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
	സ്ഥിരസ്ഥിതി:
		തിരികെ 0;
	}
}

സ്ഥിതം ശൂന്യം metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

സ്ഥിതം ഘടന sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ metanoia_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			       സ്ഥിരം ഘടന fdt_match *match)
{
	u64 reg_addr;
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 || !reg_addr)
		തിരികെ SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
