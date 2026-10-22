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
				ثابت بنية fdt_سمون *سمون)
{
	بنية platform_uart_معلومات uart = { 0 };
	ulong caps = (ulong)سمون->معلومات;
	صحيح rc;

	rc = fdt_parse_uart_غوټه(fdt, nodeoff, &uart);
	إذا (rc)
		أرجع rc;

	أرجع uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_ښويدنه, uart.reg_io_پلنوالی,
			     uart.reg_دننه_واټن, caps);
}

ساكن ثابت بنية fdt_سمون serial_uart8250_سمون[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .معلومات = (فراغ *)UART_CAP_UUE },
	{ },
};

ثابت بنية fdt_driver fdt_serial_uart8250 = {
	.سمون_جدول = serial_uart8250_سمون,
	.init = serial_uart8250_init,
};
