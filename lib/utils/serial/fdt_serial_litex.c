/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/litex-uart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_litex_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (nodeoff < 0 || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_addr || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ litex_uart_init(reg_addr);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_litex_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_litex = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_litex_ಹೊಂದಿಕೆ,
	.init = serial_litex_init
};
