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

स्थिर पूर्णाङ्क serial_uart8250_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना platform_uart_दत्त uart = { 0 };
	ulong caps = (ulong)मेलनम्->दत्त;
	पूर्णाङ्क rc;

	rc = fdt_parse_uart_ग्रन्थिः(fdt, nodeoff, &uart);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_सरणम्, uart.reg_io_विस्तारः,
			     uart.reg_विस्थापनम्, caps);
}

स्थिर ध्रुव संरचना fdt_मेलनम् serial_uart8250_मेलनम्[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .दत्त = (शून्य *)UART_CAP_UUE },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_uart8250 = {
	.मेलनम्_सारणी = serial_uart8250_मेलनम्,
	.init = serial_uart8250_init,
};
