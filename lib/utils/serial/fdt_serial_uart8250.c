/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/uart8250.h>

ਸਥਿਰ ਪੂਰਨ serial_uart8250_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };
	ulong caps = (ulong)ਮੇਲ->ਅੰਕੜੇ;
	ਪੂਰਨ rc;

	rc = fdt_parse_uart_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ uart8250_init(uart.addr, uart.freq, uart.baud,
			     uart.reg_ਖਿਸਕਾਓ, uart.reg_io_ਚੌੜਾਈ,
			     uart.reg_ਵਿੱਥ, caps);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_uart8250_ਮੇਲ[] = {
	{ .compatible = "ns16550" },
	{ .compatible = "ns16550a" },
	{ .compatible = "snps,dw-apb-uart" },
	{ .compatible = "intel,xscale-uart",
	  .ਅੰਕੜੇ = (ਖਾਲੀ *)UART_CAP_UUE },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_uart8250 = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_uart8250_ਮੇਲ,
	.init = serial_uart8250_init,
};
