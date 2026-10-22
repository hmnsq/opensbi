/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/xlnx_uartlite.h>

ਸਥਿਰ ਪੂਰਨ serial_xlnx_uartlite_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };

	rc = fdt_parse_xlnx_uartlite_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ xlnx_uartlite_init(uart.addr);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_xlnx_uartlite_ਮੇਲ[] = {
	{ .compatible = "xlnx,xps-uartlite-1.00.a" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_xlnx_uartlite = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_xlnx_uartlite_ਮੇਲ,
	.init = serial_xlnx_uartlite_init,
};
