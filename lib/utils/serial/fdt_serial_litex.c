/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/litex-uart.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_litex_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (nodeoff < 0 || !fdt)
		തിരികെ SBI_ENODEV;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_addr || !reg_size)
		തിരികെ SBI_ENODEV;

	തിരികെ litex_uart_init(reg_addr);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
