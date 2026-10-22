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
		पटिनिवत्त 1;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		पटिनिवत्त 255;
	}

	पटिनिवत्त 0;
}

स्थिर शून्य p1_ops(uint32_t type)
{
	uint8_t byte;
	पूर्णाङ्क rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	सचे (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		पटिनिवत्त;
	}

	सचे (type == SBI_SRST_RESET_TYPE_SHUTDOWN)
		byte |= PWR_CTRL2_SHUTDOWN;
	अञ्ञथा
		byte |= PWR_CTRL2_RST;

	rc = i2c_adapter_reg_write(p1_adapter, p1_reg, PWR_CTRL2, byte);
	सचे (rc)
		sbi_printf("%s: cannot write P1 Power Control Register 2\n", __func__);
}

स्थिर शून्य p1_system_reset(uint32_t type, uint32_t reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		p1_ops(type);
		भङ्ग;
	}
}

स्थिर संरचना sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

स्थिर पूर्णाङ्क p1_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			 ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &addr, रिक्त);
	सचे (rc)
		पटिनिवत्त rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_offset(fdt, nodeoff);
	सचे (i2c_bus < 0)
		पटिनिवत्त i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	सचे (rc)
		पटिनिवत्त rc;

	sbi_system_reset_add_device(&p1_reset);

	पटिनिवत्त 0;
}

स्थिर ध्रुव संरचना fdt_match p1_reset_match[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_reset_spacemit_p1 = {
	.match_table = p1_reset_match,
	.init = p1_reset_init,
};
