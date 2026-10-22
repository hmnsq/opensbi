/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/uart8250.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_uart8250_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ platform_uart_ದತ್ತಾಂಶ uart = { 0 };
	ulong caps = (ulong)ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_parse_uart_ಗಂಟು(fdt, nodeoff, &uart);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_ಸರಿಸು, uart.reg_io_ಅಗಲ,
			     uart.reg_ಸ್ಥಾನಾಂತರ, caps);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_uart8250_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .ದತ್ತಾಂಶ = (ಶೂನ್ಯ *)UART_CAP_UUE },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_uart8250 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_uart8250_ಹೊಂದಿಕೆ,
	.init = serial_uart8250_init,
};
