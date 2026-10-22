/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/gaisler-uart.h>

ਸਥਿਰ ਪੂਰਨ serial_gaisler_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			       ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };

	rc = fdt_parse_gaisler_uart_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ gaisler_uart_init(uart.addr, uart.freq, uart.baud);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_gaisler_ਮੇਲ[] = {
	{ .compatible = "gaisler,apbuart" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_gaisler = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_gaisler_ਮੇਲ,
	.init = serial_gaisler_init
};
