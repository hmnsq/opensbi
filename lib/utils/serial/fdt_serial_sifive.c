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
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/sifive-uart.h>

ਸਥਿਰ ਪੂਰਨ serial_sifive_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };

	rc = fdt_parse_sifive_uart_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ sifive_uart_init(uart.addr, uart.freq, uart.baud);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_sifive_ਮੇਲ[] = {
	{ .compatible = "sifive,fu540-c000-uart" },
	{ .compatible = "sifive,uart0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_sifive = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_sifive_ਮੇਲ,
	.init = serial_sifive_init
};
