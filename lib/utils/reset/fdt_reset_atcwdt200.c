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

#ವ್ಯಾಖ್ಯಾನಿಸು ATCWDT200_WP_ಅಂಕೆ 0x5aa5
#ವ್ಯಾಖ್ಯಾನಿಸು WREN_REG 0x18
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_REG 0x10
#ವ್ಯಾಖ್ಯಾನಿಸು RST_ಸಮಯ_OFF 8
#ವ್ಯಾಖ್ಯಾನಿಸು RST_ಸಮಯ_MSK (0x3 << RST_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು RST_CLK_128 (0 << RST_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು RST_CLK_256 (1 << RST_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು RST_CLK_512 (2 << RST_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು RST_CLK_1024 (3 << RST_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_ಸಮಯ_OFF 4
#ವ್ಯಾಖ್ಯಾನಿಸು INT_ಸಮಯ_MSK (0xf << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_64 (0 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_256 (1 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_1024 (2 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_2048 (3 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_4096 (4 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_8192 (5 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_16384 (6 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_CLK_32768 (7 << INT_ಸಮಯ_OFF)
#ವ್ಯಾಖ್ಯಾನಿಸು RST_EN (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು INT_EN (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು CLK_PCLK (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು WDT_EN (1 << 0)

#ವ್ಯಾಖ್ಯಾನಿಸು AE350_FLASH_ತಳ 0x80000000

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *wdt_addr = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ae350_system_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		ಆದರೆ (atcsmu_ಗಣ_reset_ಸದಿಶ(AE350_FLASH_ತಳ, i))
			ಹೋಗು fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_ಅಂಕೆ, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ atcwdt200_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 || !reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	wdt_addr = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr;
	sbi_system_reset_ಕೂಡಿಸು_device(&atcwdt200_reset);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ atcwdt200_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_atcwdt200 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = atcwdt200_reset_ಹೊಂದಿಕೆ,
	.init	     = atcwdt200_reset_init,
};
