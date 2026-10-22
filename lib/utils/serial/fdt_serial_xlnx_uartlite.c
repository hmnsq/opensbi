/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/xlnx_uartlite.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_xlnx_uartlite_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ platform_uart_ದತ್ತಾಂಶ uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_ಗಂಟು(fdt, nodeoff, &uart);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ xlnx_uartlite_init(uart.addr);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_xlnx_uartlite_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_xlnx_uartlite = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_xlnx_uartlite_ಹೊಂದಿಕೆ,
	.init = serial_xlnx_uartlite_init,
};
