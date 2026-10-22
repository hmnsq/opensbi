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
#تعریف_کرو PWR_CTRL2		0x7e
#تعریف_کرو PWR_CTRL2_SHUTDOWN	ثنائی_ہندسہ(2)	/* Shutdown request */
#تعریف_کرو PWR_CTRL2_RST		ثنائی_ہندسہ(1)	/* Reset request */

ساکن ساخت i2c_adapter *p1_adapter = تہی;
ساکن uint32_t p1_reg = 0;

ساکن صحیح p1_system_reset_check(uint32_t type, uint32_t reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		واپس 1;
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		واپس 255;
	}

	واپس 0;
}

ساکن خالی p1_ops(uint32_t type)
{
	uint8_t byte;
	صحیح rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	اگر (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		واپس;
	}

	اگر (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	ورنہ
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	اگر (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

ساکن خالی p1_system_reset(uint32_t type, uint32_t reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		توڑو;
	}
}

ساکن ساخت sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

ساکن صحیح p1_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			 ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, تہی);
	اگر (rc)
		واپس rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_والد_انحراف(fdt, nodeoff);
	اگر (i2c_bus < 0)
		واپس i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	اگر (rc)
		واپس rc;

	sbi_system_reset_جوڑو_device(&p1_reset);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت p1_reset_مطابقت[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_spacemit_p1 = {
	.مطابقت_جدول = p1_reset_مطابقت,
	.init = p1_reset_init,
};
