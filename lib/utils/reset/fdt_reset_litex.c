/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

ಸ್ಥಿರ ಚಞ್ಚಲ u32 *litex_soc_base;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ litex_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 255;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ litex_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			    ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 || !reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;


	litex_soc_base = (ಚಞ್ಚಲ u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
