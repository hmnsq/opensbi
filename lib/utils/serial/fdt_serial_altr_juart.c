/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/serial/fdt_serial.h>
#અન્તર્ભાવ <sbi_utils/serial/altr-juart.h>

સ્થિર પૂર્ણાંક serial_altr_juart_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				  અચલ સંરચના fdt_match *match)
{
	uint64_t reg_addr, reg_size;
	પૂર્ણાંક rc;

	જો (nodeoff < 0 || !fdt)
		પાછા SBI_ENODEV;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	/* Two 32-bit registers */
	જો (rc < 0 || !reg_addr || !reg_size || reg_size < 0x8)
		પાછા SBI_ENODEV;

	પાછા altr_juart_init(reg_addr);
}

સ્થિર અચલ સંરચના fdt_match serial_altr_juart_match[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

અચલ સંરચના fdt_driver fdt_serial_altr_juart = {
	.match_table = serial_altr_juart_match,
	.init = serial_altr_juart_init,
};
