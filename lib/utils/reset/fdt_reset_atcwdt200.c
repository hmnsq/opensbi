/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_ಅಂಕೆ 0x5aa5
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

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *wdt_addr = ರಿಕ್ತ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ae350_system_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		ಯದಿ (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			ಗಚ್ಛ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_ಅಂಕೆ, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ atcwdt200_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 || !reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	wdt_addr = (ಚಞ್ಚಲ ವರ್ಣ *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
