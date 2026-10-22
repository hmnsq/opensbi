// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/renesas-scif.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_renesas_scif_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				    സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന platform_uart_data uart = { 0 };
	പൂർണ്ണസംഖ്യ ret;

	ret = fdt_parse_renesas_scif_കണ്ണി(fdt, nodeoff, &uart);
	എങ്കിൽ (ret)
		തിരികെ ret;

	തിരികെ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
