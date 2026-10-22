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

#ವ್ಯಾಖ್ಯಾನಿಸು RESET_CTRL			0x0

ಸ್ಥಿರ ಅಸ್ಥಿರ u32 *litex_soc_ತಳ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ litex_reset_check(u32 type, u32 reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಹಿಂತಿರುಗಿ 255;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_ತಳ + RESET_CTRL);
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ litex_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			    ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	uint64_t reg_addr;
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, ಖಾಲಿ);
	ಆದರೆ (rc < 0 || !reg_addr)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;


	litex_soc_ತಳ = (ಅಸ್ಥಿರ u32 *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr;

	sbi_system_reset_ಕೂಡಿಸು_device(&litex_reset);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ litex_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_litex = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = litex_reset_ಹೊಂದಿಕೆ,
	.init = litex_reset_init,
};
