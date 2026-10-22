/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/cadence-uart.h>

ਸਥਿਰ ਪੂਰਨ serial_cadence_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			       ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };

	rc = fdt_parse_uart_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ cadence_uart_init(uart.addr, uart.freq, uart.baud);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_cadence_ਮੇਲ[] = {
	{ .compatible = "cdns,uart-r1p8", },
	{ .compatible = "cdns,uart-r1p12" },
	{ .compatible = "starfive,jh8100-uart" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_cadence = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_cadence_ਮੇਲ,
	.init = serial_cadence_init
};
