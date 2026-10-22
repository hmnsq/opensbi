/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/fdt_serial.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/altr-juart.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serial_altr_juart_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				  ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (nodeoff < 0 || !fdt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, &reg_ಗಾತ್ರ);
	/* Two 32-bit registers */
	ಆದರೆ (rc < 0 || !reg_addr || !reg_ಗಾತ್ರ || reg_ಗಾತ್ರ < 0x8)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ altr_juart_init(reg_addr);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ serial_altr_juart_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "altr,juart-1.0" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_serial_altr_juart = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = serial_altr_juart_ಹೊಂದಿಕೆ,
	.init = serial_altr_juart_init,
};
