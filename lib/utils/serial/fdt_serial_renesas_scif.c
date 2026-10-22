// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/renesas-scif.h>

स्थिर पूर्णाङ्क serial_renesas_scif_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				    अचर संरचना fdt_match *match)
{
	संरचना platform_uart_data uart = { 0 };
	पूर्णाङ्क ret;

	ret = fdt_parse_renesas_scif_गाँठो(fdt, nodeoff, &uart);
	यदि (ret)
		फर्काऊ ret;

	फर्काऊ renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

स्थिर अचर संरचना fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

अचर संरचना fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
