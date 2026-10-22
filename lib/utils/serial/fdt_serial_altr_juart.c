/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/serial/fdt_serial.h>
#അന്തര്ഭാവ <sbi_utils/serial/altr-juart.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ serial_altr_juart_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				  സ്ഥിരം ഘടന fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (nodeoff < 0 || !fdt)
		തിരികെ SBI_ENODEV;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	എങ്കിൽ (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		തിരികെ SBI_ENODEV;

	തിരികെ altr_juart_init(reg_addr);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
