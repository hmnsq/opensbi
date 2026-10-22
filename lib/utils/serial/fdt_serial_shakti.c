/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/shakti-uart.h>

ਸਥਿਰ ਪੂਰਨ serial_shakti_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };

	rc = fdt_parse_shakti_uart_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ shakti_uart_init(uart.addr, uart.freq, uart.baud);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_shakti_ਮੇਲ[] = {
	{ .compatible = "shakti,uart0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_shakti = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_shakti_ਮੇਲ,
	.init = serial_shakti_init
};
