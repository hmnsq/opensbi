/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/litex-uart.h>

நிலையான முழு serial_litex_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	முழு rc;

	எனில் (nodeoff < 0 || !fdt)
		திரும்பு SBI_ENODEV;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_addr || !reg_size)
		திரும்பு SBI_ENODEV;

	திரும்பு litex_uart_init(reg_addr);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_litex_match[] = {
	{ .compatible = "litex,liteuart" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_litex = {
	.match_table = serial_litex_match,
	.init = serial_litex_init
};
