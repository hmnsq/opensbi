/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/litex-uart.h>

अटल पूर्णांक serial_litex_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	uint64_t reg_addr, reg_आकार;
	पूर्णांक rc;

	अगर (nodeoff < 0 || !fdt)
		वापस SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_addr || !reg_आकार)
		वापस SBI_ENODEV;

	वापस litex_uart_init(reg_addr);
}

अटल स्थिर काष्ठा fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
