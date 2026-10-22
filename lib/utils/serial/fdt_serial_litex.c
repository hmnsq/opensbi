/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/litex-uart.h>

ਸਥਿਰ ਪੂਰਨ serial_litex_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	uint64_t reg_addr, reg_ਮਾਪ;
	ਪੂਰਨ rc;

	ਜੇ (nodeoff < 0 || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_addr || !reg_ਮਾਪ)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ litex_uart_init(reg_addr);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_litex_ਮੇਲ[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_litex = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_litex_ਮੇਲ,
	.init = serial_litex_init
};
