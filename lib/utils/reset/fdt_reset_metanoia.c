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

#ವ್ಯಾಖ್ಯಾನಿಸು SWRSTREQ_CTRL_REG_ಸ್ಥಾನಾಂತರ 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು SWRSTREQ_REG_ಸ್ಥಾನಾಂತರ 0x04

ರಚನೆ reset_metanoia_ದತ್ತಾಂಶ {
	u8 *reg_ತಳ;
};

ಸ್ಥಿರ ರಚನೆ reset_metanoia_ದತ್ತಾಂಶ reset_ದತ್ತಾಂಶ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ metanoia_system_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ 0;
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_ದತ್ತಾಂಶ.reg_ತಳ + SWRSTREQ_CTRL_REG_ಸ್ಥಾನಾಂತರ);
	writew(0x1, reset_ದತ್ತಾಂಶ.reg_ತಳ + SWRSTREQ_REG_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ metanoia_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			       ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	u64 reg_addr;
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 || !reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	reset_ದತ್ತಾಂಶ.reg_ತಳ = (u8 *)(ulong)reg_addr;

	sbi_system_reset_ಕೂಡಿಸು_device(&metanoia_reset);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ metanoia_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_metanoia = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = metanoia_reset_ಹೊಂದಿಕೆ,
	.init	     = metanoia_reset_init,
};
