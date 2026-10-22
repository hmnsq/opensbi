/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#ضمن <libfdt.h>
#ضمن <platform_override.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

ساكن بنية i2c_adapter *p1_adapter = عدم;
ساكن uint32_t p1_reg = 0;

ساكن صحيح p1_system_reset_check(uint32_t type, uint32_t reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		أرجع 1;
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		أرجع 255;
	}

	أرجع 0;
}

ساكن فراغ p1_ops(uint32_t type)
{
	uint8_t byte;
	صحيح rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	إذا (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		أرجع;
	}

	إذا (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	وإلا
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	إذا (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ساكن فراغ p1_system_reset(uint32_t type, uint32_t reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		اكسر;
	}
}

ساكن بنية sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ساكن صحيح p1_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
			 ثابت بنية fdt_match *match)
{
	صحيح rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_عقدة_addr_size(fdt, nodeoff, 0, &addr, عدم);
	إذا (rc)
		أرجع rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	إذا (i2c_bus < 0)
		أرجع i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	إذا (rc)
		أرجع rc;

	sbi_system_reset_add_device(&p1_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ثابت بنية fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
