/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/shakti-uart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_shakti_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ platform_uart_ದತ್ತಾಂಶ uart = { 0 };

	rc = fdt_parse_shakti_uart_ಗಂಟು(fdt, nodeoff, &uart);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_shakti_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_shakti = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_shakti_ಹೊಂದಿಕೆ,
	.init = serial_shakti_init
};
