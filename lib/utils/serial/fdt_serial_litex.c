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

स्थिर पूर्णाङ्क serial_litex_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			     ध्रुव संरचना fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	पूर्णाङ्क rc;

	यदि (nodeoff < 0 || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_addr || !reg_size)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम litex_uart_init(reg_addr);
}

स्थिर ध्रुव संरचना fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
