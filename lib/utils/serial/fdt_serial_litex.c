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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_litex_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			     ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (nodeoff < 0 || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_addr || !reg_size)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ litex_uart_init(reg_addr);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
