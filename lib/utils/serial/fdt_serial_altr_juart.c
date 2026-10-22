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

स्थिर पूर्णांक serial_altr_juart_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				  स्थिरांक संरचना fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	पूर्णांक rc;

	जर (nodeoff < 0 || !fdt)
		परत SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	जर (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		परत SBI_ENODEV;

	परत altr_juart_init(reg_addr);
}

स्थिर स्थिरांक संरचना fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
