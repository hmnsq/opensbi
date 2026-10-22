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
#परिभाषय PWR_CTRL2		0x7e
#परिभाषय PWR_CTRL2_SHUTDOWN	BIT(2)	/* Shutdown request */
#परिभाषय PWR_CTRL2_RST		BIT(1)	/* Reset request */

स्थिर संरचना i2c_adapter *p1_adapter = रिक्त;
स्थिर uint32_t p1_reg = 0;

स्थिर पूर्णाङ्क p1_system_reset_check(uint32_t type, uint32_t reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
		प्रत्यागम 1;
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
		प्रत्यागम 255;
	}

	प्रत्यागम 0;
}

स्थिर शून्य p1_ops(uint32_t type)
{
	uint8_t byte;
	पूर्णाङ्क rc;

	rc = i2c_adapter_reg_read(p1_adapter, p1_reg, PWR_CTRL2, &byte);
	यदि (rc) {
		sbi_printf("%s: cannot read P1 Power Control Register 2\n", __func__);
		प्रत्यागम;
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
		भङ्ग;
	}
}

स्थिर संरचना sbi_system_reset_device p1_reset = {
	.name = "spacemit-p1-reset",
	.system_reset_check = p1_system_reset_check,
	.system_reset = p1_system_reset
};

स्थिर पूर्णाङ्क p1_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			 ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc, i2c_bus;
	uint64_t addr;

	/* we are spacemit,p1 node */
	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &addr, रिक्त);
	यदि (rc)
		प्रत्यागम rc;

	p1_reg = addr;

	/* find i2c bus parent node */
	i2c_bus = fdt_parent_विस्थापनम्(fdt, nodeoff);
	यदि (i2c_bus < 0)
		प्रत्यागम i2c_bus;

	/* i2c adapter get */
	rc = fdt_i2c_adapter_get(fdt, i2c_bus, &p1_adapter);
	यदि (rc)
		प्रत्यागम rc;

	sbi_system_reset_सङ्कलय_device(&p1_reset);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् p1_reset_मेलनम्[] = {
	{ .compatible = "spacemit,p1" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_reset_spacemit_p1 = {
	.मेलनम्_सारणी = p1_reset_मेलनम्,
	.init = p1_reset_init,
};
