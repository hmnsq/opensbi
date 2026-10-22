/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/serial/fdt_serial.h>
#समावेश <sbi_utils/serial/altr-juart.h>

अटल पूर्णांक serial_altr_juart_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				  स्थिर काष्ठा fdt_match *match)
{
	uint64_t reg_addr, reg_आकार;
	पूर्णांक rc;

	अगर (nodeoff < 0 || !fdt)
		वापस SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	/* Two 32-bit registers */
	अगर (rc < 0 || !reg_addr || !reg_आकार || reg_आकार < 0x8)
		वापस SBI_ENODEV;

	वापस altr_juart_init(reg_addr);
}

अटल स्थिर काष्ठा fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
