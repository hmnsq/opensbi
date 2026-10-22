/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/altr-juart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serial_altr_juart_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				  ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (nodeoff < 0 || !fdt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	ಯದಿ (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ altr_juart_init(reg_addr);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
