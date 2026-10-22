// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/renesas-scif.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_renesas_scif_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				    ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ platform_uart_data uart = { 0 };
	ಪೂರ್ಣಾಙ್ಕ ret;

	ret = fdt_parse_renesas_scif_ಗಂಟು(fdt, nodeoff, &uart);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	ಪ್ರತ್ಯಾಗಮ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
