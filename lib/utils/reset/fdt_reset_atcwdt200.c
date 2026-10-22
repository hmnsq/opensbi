/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_અંક 0x5aa5
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

સ્થિર અસ્થિર અક્ષર *wdt_addr = ખાલી;

સ્થિર પૂર્ણાંક ae350_system_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		પાછા 1;
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	મૂળભૂત:
		પાછા 0;
	}
}

સ્થિર શૂન્ય ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		જો (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			જાઓ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_અંક, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

સ્થિર સંરચના sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

સ્થિર પૂર્ણાંક atcwdt200_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				અચલ સંરચના fdt_match *match)
{
	uint64_t reg_addr;
	પૂર્ણાંક rc;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 || !reg_addr)
		પાછા SBI_ENODEV;

	wdt_addr = (અસ્થિર અક્ષર *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

અચલ સંરચના fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
