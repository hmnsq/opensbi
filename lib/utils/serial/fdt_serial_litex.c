/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/serial/fdt_serial.h>
#شامل <sbi_utils/serial/litex-uart.h>

ایستا صحیح serial_litex_init(ثابت تهی *fdt, صحیح nodeoff,
			     ثابت ساختار fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	صحیح rc;

	اگر (nodeoff < 0 || !fdt)
		بازگشت SBI_ENODEV;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_addr || !reg_size)
		بازگشت SBI_ENODEV;

	بازگشت litex_uart_init(reg_addr);
}

ایستا ثابت ساختار fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ثابت ساختار fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
