/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Aurelien Jarno
 * Authors:
 *   Aurelien Jarno <aurelien@aurel32.net>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <platform_override.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/i2c/fdt_i2c.h>

/* SpacemiT P1 Power Control Register 2 */
#define PWR_CTRL2		0x7e
#define PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#define PWR_CTRL2_RST		BIT(1)	/* Reset request */

स्थिर संरचना i2c_adapter *p1_adapter = रिक्त;
स्थिर uint32_t p1_reg = 0;

स्थिर पूर्णाङ्क p1_system_reset_check(uint32_t type, uint32_t reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		फर्काऊ 1;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		फर्काऊ 255;
	}

	फर्काऊ 0;
}

स्थिर शून्य p1_ops(uint32_t type)
{
	uint8_t byte;
	पूर्णाङ्क rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	यदि (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		फर्काऊ;
	}

	यदि (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	अन्यथा
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	यदि (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

स्थिर शून्य p1_system_reset(uint32_t type, uint32_t reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		रोक;
	}
}

स्थिर संरचना sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

स्थिर पूर्णाङ्क p1_reset_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
			 अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc)
		फर्काऊ rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	यदि (i2c_bus < 0)
		फर्काऊ i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	यदि (rc)
		फर्काऊ rc;

	sbi_system_reset_add_device(&p1_reset);

	फर्काऊ 0;
}

स्थिर अचर संरचना fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

अचर संरचना fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
