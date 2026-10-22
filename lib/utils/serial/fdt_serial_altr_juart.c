/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/serial/fdt_serial.h>
#அந்தர்பாவ <sbi_utils/serial/altr-juart.h>

நிலையான முழு serial_altr_juart_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				  மாறிலி கட்டமைப்பு fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	முழு rc;

	எனில் (nodeoff < 0 || !fdt)
		திரும்பு SBI_ENODEV;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	எனில் (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		திரும்பு SBI_ENODEV;

	திரும்பு altr_juart_init(reg_addr);
}

நிலையான மாறிலி கட்டமைப்பு fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
