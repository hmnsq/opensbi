/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/sys/htif.h>

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_htif_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "ucb,htif0" },
	{ },
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_htif_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			    ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	bool custom = false;
	uint64_t fromhost_addr = 0, tohost_addr = 0;

	ಆದರೆ (!fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &fromhost_addr, ಖಾಲಿ)) {
		custom = true;
		tohost_addr = fromhost_addr + ಗಾತ್ರ(uint64_t);
	}

	fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 1, &tohost_addr, ಖಾಲಿ);

	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(fromhost_addr, ಪುಟ_ಗಾತ್ರ, ಪುಟ_ಗಾತ್ರ,
					  (SBI_DOMAIN_MEMREGION_MMIO |
					   SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ htif_serial_init(custom, fromhost_addr, tohost_addr);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_htif = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_htif_ಹೊಂದಿಕೆ,
	.init = serial_htif_init
};
