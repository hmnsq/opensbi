/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

ಸಂರಚನಾ reset_metanoia_data {
	u8 *reg_base;
};

ಸ್ಥಿರ ಸಂರಚನಾ reset_metanoia_data reset_data;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ metanoia_system_reset_check(u32 type, u32 reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ metanoia_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			       ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	u64 reg_addr;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, ರಿಕ್ತ);
	ಯದಿ (rc < 0 || !reg_addr)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
