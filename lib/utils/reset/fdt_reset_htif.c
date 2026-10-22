/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/sys/htif.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ htif_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ಆದರೆ (!fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &fromhost_addr, ಖಾಲಿ)) {
		custom = true;
		tohost_addr = fromhost_addr + ಗಾತ್ರ(uint64_t);
	}

	fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 1, &tohost_addr, ಖಾಲಿ);

	ಹಿಂತಿರುಗಿ htif_system_reset_init(custom, fromhost_addr, tohost_addr);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ htif_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_htif = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = htif_reset_ಹೊಂದಿಕೆ,
	.init = htif_reset_init
};
