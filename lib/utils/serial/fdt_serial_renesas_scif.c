// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/renesas-scif.h>

நிலையான முழு serial_renesas_scif_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				    மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு platform_uart_data uart = { 0 };
	முழு ret;

	ret = fdt_parse_renesas_scif_முனை(fdt, nodeoff, &uart);
	எனில் (ret)
		திரும்பு ret;

	திரும்பு renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
