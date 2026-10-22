/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/litex-uart.h>

स्थिर पूर्णांक serial_litex_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			     स्थिरांक संरचना fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	पूर्णांक rc;

	जर (nodeoff < 0 || !fdt)
		परत SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	जर (rc < 0 || !reg_addr || !reg_size)
		परत SBI_ENODEV;

	परत litex_uart_init(reg_addr);
}

स्थिर स्थिरांक संरचना fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
