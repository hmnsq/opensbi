// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/renesas-scif.h>

ساكن صحيح serial_renesas_scif_init(ثابت فراغ *fdt, صحيح nodeoff,
				    ثابت بنية fdt_سمون *سمون)
{
	بنية platform_uart_معلومات uart = { 0 };
	صحيح ret;

	ret = fdt_parse_renesas_scif_غوټه(fdt, nodeoff, &uart);
	إذا (ret)
		أرجع ret;

	أرجع renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ساكن ثابت بنية fdt_سمون serial_renesas_scif_سمون[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_serial_renesas_scif = {
	.سمون_جدول = serial_renesas_scif_سمون,
	.init = serial_renesas_scif_init
};
