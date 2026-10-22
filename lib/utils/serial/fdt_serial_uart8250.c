/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/uart8250.h>

ساكن صحيح serial_uart8250_init(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت بنية fdt_میل *میل)
{
	بنية platform_uart_انکڑے uart = { 0 };
	ulong caps = (ulong)میل->انکڑے;
	صحيح rc;

	rc = fdt_parse_uart_گرہ(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_کھسکاؤ, uart.reg_io_چوڑائی,
			     uart.reg_وتھ, caps);
}

ساكن ثابت بنية fdt_میل serial_uart8250_میل[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .انکڑے = (فراغ *)UART_CAP_UUE },
	{ },
};

ثابت بنية fdt_driver fdt_serial_uart8250 = {
	.میل_جدول = serial_uart8250_میل,
	.init = serial_uart8250_init,
};
