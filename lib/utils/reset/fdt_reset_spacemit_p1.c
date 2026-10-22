/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <platform_override.h>
#অন্তর্ভাৱ <sbi/sbi_bitops.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাৱ <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

স্থির সংরচনা i2c_adapter *p1_adapter = রিক্ত;
স্থির uint32_t p1_reg = 0;

স্থির পূর্ণাঙ্ক p1_system_reset_check(uint32_t type, uint32_t reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ঘূৰাই 1;
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ঘূৰাই 255;
	}

	ঘূৰাই 0;
}

স্থির শূন্য p1_ops(uint32_t type)
{
	uint8_t byte;
	পূর্ণাঙ্ক rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	যদি (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		ঘূৰাই;
	}

	যদি (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	অন্যথা
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	যদি (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

স্থির শূন্য p1_system_reset(uint32_t type, uint32_t reason)
{
	ৱিকল্প (type) {
	পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
	পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		ভঙ্গ;
	}
}

স্থির সংরচনা sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

স্থির পূর্ণাঙ্ক p1_reset_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			 ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &addr, রিক্ত);
	যদি (rc)
		ঘূৰাই rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	যদি (i2c_bus < 0)
		ঘূৰাই i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	যদি (rc)
		ঘূৰাই rc;

	sbi_system_reset_add_device(&p1_reset);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
