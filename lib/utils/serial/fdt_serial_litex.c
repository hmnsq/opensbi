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
			     ثابت بنية fdt_تطابق *تطابق)
{
	uint64_t reg_addr, reg_اندازه;
	صحيح rc;

	إذا (nodeoff < 0 || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_addr || !reg_اندازه)
		أرجع SBI_ENODEV;

	أرجع litex_uart_init(reg_addr);
}

ساكن ثابت بنية fdt_تطابق serial_litex_تطابق[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ثابت بنية fdt_driver fdt_serial_litex = {
	.تطابق_جدول = serial_litex_تطابق,
	.init = serial_litex_init
};
