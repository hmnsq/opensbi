// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/renesas-scif.h>

ਸਥਿਰ ਪੂਰਨ serial_renesas_scif_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				    ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ platform_uart_ਅੰਕੜੇ uart = { 0 };
	ਪੂਰਨ ret;

	ret = fdt_parse_renesas_scif_ਗੰਢ(fdt, nodeoff, &uart);
	ਜੇ (ret)
		ਵਾਪਸ ret;

	ਵਾਪਸ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_renesas_scif_ਮੇਲ[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_renesas_scif = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_renesas_scif_ਮੇਲ,
	.init = serial_renesas_scif_init
};
