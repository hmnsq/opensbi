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
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

ಸ್ಥಿರ ಸಂರಚನಾ i2c_adapter *p1_adapter = ರಿಕ್ತ;
ಸ್ಥಿರ uint32_t p1_reg = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ಪ್ರತ್ಯಾಗಮ 255;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ p1_ops(uint32_t type)
{
	uint8_t byte;
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	ಯದಿ (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ಅನ್ಯಥಾ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	ಯದಿ (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ಸ್ಥಿರ ಶೂನ್ಯ p1_system_reset(uint32_t type, uint32_t reason)
{
	ವಿಕಲ್ಪ (type) {
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ಭಙ್ಗ;
	}
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ p1_reset_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			 ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, ರಿಕ್ತ);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	ಯದಿ (i2c_bus < 0)
		ಪ್ರತ್ಯಾಗಮ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sbi_system_reset_add_device(&p1_reset);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
