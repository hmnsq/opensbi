/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/serial/fdt_serial.h>
#ضمن <sbi_utils/serial/litex-uart.h>

ساكن صحيح serial_litex_init(ثابت فراغ *fdt, صحيح nodeoff,
			     ثابت بنية fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	صحيح rc;

	إذا (nodeoff < 0 || !fdt)
		إرجاع SBI_ENODEV;

	rc = fdt_get_عقدة_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	إذا (rc < 0 || !reg_addr || !reg_size)
		إرجاع SBI_ENODEV;

	إرجاع litex_uart_init(reg_addr);
}

ساكن ثابت بنية fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
