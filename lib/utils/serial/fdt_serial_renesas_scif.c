// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/renesas-scif.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_renesas_scif_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				    ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ platform_uart_ದತ್ತಾಂಶ uart = { 0 };
	ಪೂರ್ಣಾಂಕ ret;

	ret = fdt_parse_renesas_scif_ಗಂಟು(fdt, nodeoff, &uart);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	ಹಿಂತಿರುಗಿ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_renesas_scif_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_renesas_scif = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_renesas_scif_ಹೊಂದಿಕೆ,
	.init = serial_renesas_scif_init
};
