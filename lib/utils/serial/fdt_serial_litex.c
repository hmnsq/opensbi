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
			     ثابت بنية fdt_میل *میل)
{
	uint64_t reg_addr, reg_ناپ;
	صحيح rc;

	إذا (nodeoff < 0 || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_addr || !reg_ناپ)
		أرجع SBI_ENODEV;

	أرجع litex_uart_init(reg_addr);
}

ساكن ثابت بنية fdt_میل serial_litex_میل[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_litex = {
	.میل_جدول = serial_litex_میل,
	.init = serial_litex_init
};
