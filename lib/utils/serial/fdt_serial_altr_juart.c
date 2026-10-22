/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/serial/fdt_serial.h>
#अन्तर्भाव <sbi_utils/serial/altr-juart.h>

स्थिर पूर्णाङ्क serial_altr_juart_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				  ध्रुव संरचना fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	पूर्णाङ्क rc;

	यदि (nodeoff < 0 || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	यदि (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम altr_juart_init(reg_addr);
}

स्थिर ध्रुव संरचना fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
