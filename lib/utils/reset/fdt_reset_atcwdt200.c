/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_അക്കം 0x5aa5
#define WREN_REG 0x18
#define CTRL_REG 0x10
#define RST_TIME_OFF 8
#define RST_TIME_MSK (0x3 << RST_TIME_OFF)
#define RST_CLK_128 (0 << RST_TIME_OFF)
#define RST_CLK_256 (1 << RST_TIME_OFF)
#define RST_CLK_512 (2 << RST_TIME_OFF)
#define RST_CLK_1024 (3 << RST_TIME_OFF)
#define INT_TIME_OFF 4
#define INT_TIME_MSK (0xf << INT_TIME_OFF)
#define INT_CLK_64 (0 << INT_TIME_OFF)
#define INT_CLK_256 (1 << INT_TIME_OFF)
#define INT_CLK_1024 (2 << INT_TIME_OFF)
#define INT_CLK_2048 (3 << INT_TIME_OFF)
#define INT_CLK_4096 (4 << INT_TIME_OFF)
#define INT_CLK_8192 (5 << INT_TIME_OFF)
#define INT_CLK_16384 (6 << INT_TIME_OFF)
#define INT_CLK_32768 (7 << INT_TIME_OFF)
#define RST_EN (1 << 3)
#define INT_EN (1 << 2)
#define CLK_PCLK (1 << 1)
#define WDT_EN (1 << 0)

#define AE350_FLASH_BASE 0x80000000

സ്ഥിതം അസ്ഥിര അക്ഷരം *wdt_addr = ശൂന്യ;

സ്ഥിതം പൂർണ്ണസംഖ്യ ae350_system_reset_check(u32 type, u32 reason)
{
	തിരഞ്ഞെടുപ്പ് (type) {
	അവസ്ഥ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		തിരികെ 1;
	അവസ്ഥ SBI_SRST_RESET_TYPE_SHUTDOWN:
	അവസ്ഥ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	സ്ഥിരസ്ഥിതി:
		തിരികെ 0;
	}
}

സ്ഥിതം ശൂന്യം ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		എങ്കിൽ (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			പോകുക fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_അക്കം, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

സ്ഥിതം ഘടന sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ atcwdt200_reset_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				സ്ഥിരം ഘടന fdt_match *match)
{
	uint64_t reg_addr;
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 || !reg_addr)
		തിരികെ SBI_ENODEV;

	wdt_addr = (അസ്ഥിര അക്ഷരം *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
