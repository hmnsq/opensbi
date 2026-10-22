/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

സ്ഥിതം അസ്ഥിര u32 *litex_soc_base;

സ്ഥിതം പൂർണ്ണസംഖ്യ litex_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		തിരികെ 255;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

സ്ഥിതം ഘടന sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

സ്ഥിതം പൂർണ്ണസംഖ്യ litex_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			    സ്ഥിരം ഘടന fdt_match *match)
{
	uint64_t reg_addr;
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 || !reg_addr)
		തിരികെ SBI_ENODEV;


	litex_soc_base = (അസ്ഥിര u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
