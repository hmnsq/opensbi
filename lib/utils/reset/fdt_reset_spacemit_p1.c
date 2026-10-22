/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <platform_override.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#ವ್ಯಾಖ್ಯಾನಿಸು PWR_CTRL2		0x7e
#ವ್ಯಾಖ್ಯಾನಿಸು PWR_CTRL2_SHUTDOWN	ಬಿಟ್(2)	/* Shutdown request */
#ವ್ಯಾಖ್ಯಾನಿಸು PWR_CTRL2_RST		ಬಿಟ್(1)	/* Reset request */

ಸ್ಥಿರ ರಚನೆ i2c_adapter *p1_adapter = ಖಾಲಿ;
ಸ್ಥಿರ uint32_t p1_reg = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಹಿಂತಿರುಗಿ 255;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ p1_ops(uint32_t type)
{
	uint8_t byte;
	ಪೂರ್ಣಾಂಕ rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ಆದರೆ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ಇಲ್ಲದಿದ್ದರೆ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ಆದರೆ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ಸ್ಥಿರ ಶೂನ್ಯ p1_system_reset(uint32_t type, uint32_t reason)
{
	ಆಯ್ಕೆ (type) {
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ನಿಲ್ಲಿಸು;
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ p1_reset_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, ಖಾಲಿ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, nodeoff);
	ಆದರೆ (i2c_bus < 0)
		ಹಿಂತಿರುಗಿ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_system_reset_ಕೂಡಿಸು_device(&p1_reset);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ p1_reset_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_reset_spacemit_p1 = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = p1_reset_ಹೊಂದಿಕೆ,
	.init = p1_reset_init,
};
