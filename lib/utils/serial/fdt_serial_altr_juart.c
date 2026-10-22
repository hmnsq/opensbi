/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/fdt_serial.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/altr-juart.h>

ਸਥਿਰ ਪੂਰਨ serial_altr_juart_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				  ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	uint64_t reg_addr, reg_ਮਾਪ;
	ਪੂਰਨ rc;

	ਜੇ (nodeoff < 0 || !fdt)
		ਵਾਪਸ SBI_ENODEV;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, &reg_ਮਾਪ);
	/* Two 32-bit registers */
	ਜੇ (rc < 0 || !reg_addr || !reg_ਮਾਪ || reg_ਮਾਪ < 0x8)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ altr_juart_init(reg_addr);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ serial_altr_juart_ਮੇਲ[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_serial_altr_juart = {
	.ਮੇਲ_ਸਾਰਣੀ = serial_altr_juart_ਮੇਲ,
	.init = serial_altr_juart_init,
};
