// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/renesas-scif.h>

अटल पूर्णांक serial_renesas_scif_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				    स्थिर काष्ठा fdt_match *match)
{
	काष्ठा platform_uart_तथ्य uart = { 0 };
	पूर्णांक ret;

	ret = fdt_parse_renesas_scif_गाँठ(fdt, nodeoff, &uart);
	अगर (ret)
		वापस ret;

	वापस renesas_scif_init(uart.addr, uart.freq, uart.baud);
}

अटल स्थिर काष्ठा fdt_match serial_renesas_scif_match[] = {
	{ .compatible = "renesas,scif-r9a07g043" },
	{ /* sentinel */ }
};

स्थिर काष्ठा fdt_driver fdt_serial_renesas_scif = {
	.match_table = serial_renesas_scif_match,
	.init = serial_renesas_scif_init
};
