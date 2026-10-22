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

स्थिर पूर्णाङ्क serial_altr_juart_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				  अचर संरचना fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	पूर्णाङ्क rc;

	यदि (nodeoff < 0 || !fdt)
		फर्काऊ SBI_ENODEV;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	यदि (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		फर्काऊ SBI_ENODEV;

	फर्काऊ altr_juart_init(reg_addr);
}

स्थिर अचर संरचना fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

अचर संरचना fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
