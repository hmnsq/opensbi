/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#شامل <libfdt.h>
#شامل <platform_override.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

ایستا ساختار i2c_adapter *p1_adapter = پوچ;
ایستا uint32_t p1_reg = 0;

ایستا صحیح p1_system_reset_check(uint32_t type, uint32_t reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		بازگشت 1;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		بازگشت 255;
	}

	بازگشت 0;
}

ایستا تهی p1_ops(uint32_t type)
{
	uint8_t byte;
	صحیح rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	اگر (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		بازگشت;
	}

	اگر (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	وگرنه
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	اگر (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ایستا تهی p1_system_reset(uint32_t type, uint32_t reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		بشکن;
	}
}

ایستا ساختار sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ایستا صحیح p1_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			 ثابت ساختار fdt_match *match)
{
	صحیح rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, پوچ);
	اگر (rc)
		بازگشت rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	اگر (i2c_bus < 0)
		بازگشت i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	اگر (rc)
		بازگشت rc;

	sbi_system_reset_add_device(&p1_reset);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
