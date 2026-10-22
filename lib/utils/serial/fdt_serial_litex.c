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
			     ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	uint64_t reg_addr, reg_आकार;
	पूर्णाङ्क rc;

	यदि (nodeoff < 0 || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_addr || !reg_आकार)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम litex_uart_init(reg_addr);
}

स्थिर ध्रुव संरचना fdt_मेलनम् serial_litex_मेलनम्[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_litex = {
	.मेलनम्_सारणी = serial_litex_मेलनम्,
	.init = serial_litex_init
};
