/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/litex-uart.h>

સ્થિર પૂર્ણાંક serial_litex_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	પૂર્ણાંક rc;

	જો (nodeoff < 0 || !fdt)
		પાછા SBI_ENODEV;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	જો (rc < 0 || !reg_addr || !reg_size)
		પાછા SBI_ENODEV;

	પાછા litex_uart_init(reg_addr);
}

સ્થિર અચલ સંરચના fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
