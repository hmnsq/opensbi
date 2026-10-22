/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/uart8250.h>

स्थिर पूर्णाङ्क serial_uart8250_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				अचर संरचना fdt_match *match)
{
	संरचना platform_uart_data uart = { 0 };
	ulong caps = (ulong)match->data;
	पूर्णाङ्क rc;

	rc = fdt_parse_uart_गाँठो(fdt, nodeoff, &uart);
	यदि (rc)
		फर्काऊ rc;

	फर्काऊ uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_shift, uart.reg_io_width,
			     uart.reg_offset, caps);
}

स्थिर अचर संरचना fdt_match serial_uart8250_match[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .data = (शून्य *)UART_CAP_UUE },
	{ },
};

अचर संरचना fdt_driver fdt_serial_uart8250 = {
	.match_table = serial_uart8250_match,
	.init = serial_uart8250_init,
};
