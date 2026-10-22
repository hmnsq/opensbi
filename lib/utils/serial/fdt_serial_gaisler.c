/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/gaisler-uart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_gaisler_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			       ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ platform_uart_ದತ್ತಾಂಶ uart = { 0 };

	rc = fdt_parse_gaisler_uart_ಗಂಟು(fdt, nodeoff, &uart);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_gaisler_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_gaisler = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_gaisler_ಹೊಂದಿಕೆ,
	.init = serial_gaisler_init
};
