// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/renesas-scif.h>

ساكن صحيح serial_renesas_scif_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_match *match)
{
	بنية platform_uart_data uart = { 0 };
	صحيح ret;

	ret = fdt_parse_renesas_scif_عقدة(fdt, nodeoff, &uart);
	إذا (ret)
		أرجع ret;

	أرجع renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
