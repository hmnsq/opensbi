// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/renesas-scif.h>

સ્થિર પૂર્ણાંક serial_renesas_scif_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				    અચલ સંરચના fdt_match *match)
{
	સંરચના platform_uart_data uart = { 0 };
	પૂર્ણાંક ret;

	ret = fdt_parse_renesas_scif_ગાંઠ(fdt, nodeoff, &uart);
	જો (ret)
		પાછા ret;

	પાછા renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

સ્થિર અચલ સંરચના fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

અચલ સંરચના fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
