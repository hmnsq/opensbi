// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/renesas-scif.h>

स्थिर पूर्णाङ्क serial_renesas_scif_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				    ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना platform_uart_दत्त uart = { 0 };
	पूर्णाङ्क ret;

	ret = fdt_parse_renesas_scif_ग्रन्थिः(fdt, nodeoff, &uart);
	यदि (ret)
		प्रत्यागम ret;

	प्रत्यागम renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

स्थिर ध्रुव संरचना fdt_मेलनम् serial_renesas_scif_मेलनम्[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

ध्रुव संरचना fdt_driver fdt_serial_renesas_scif = {
	.मेलनम्_सारणी = serial_renesas_scif_मेलनम्,
	.init = serial_renesas_scif_init
};
