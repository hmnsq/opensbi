/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

#परिभाषय SWRSTREQ_CTRL_REG_विस्थापनम् 0x00
#परिभाषय SWRSTREQ_REG_विस्थापनम् 0x04

संरचना reset_metanoia_दत्त {
	u8 *reg_आधारः;
};

स्थिर संरचना reset_metanoia_दत्त reset_दत्त;

स्थिर पूर्णाङ्क metanoia_system_reset_check(u32 type, u32 reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
		प्रत्यागम 1;
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
	स्वभाव:
		प्रत्यागम 0;
	}
}

स्थिर शून्य metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_दत्त.reg_आधारः + SWRSTREQ_CTRL_REG_विस्थापनम्);
	writew(0x1, reset_दत्त.reg_आधारः + SWRSTREQ_REG_विस्थापनम्);
}

स्थिर संरचना sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

स्थिर पूर्णाङ्क metanoia_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			       ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	u64 reg_addr;
	पूर्णाङ्क rc;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, रिक्त);
	यदि (rc < 0 || !reg_addr)
		प्रत्यागम SBI_ENODEV;

	reset_दत्त.reg_आधारः = (u8 *)(ulong)reg_addr;

	sbi_system_reset_सङ्कलय_device(&metanoia_reset);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् metanoia_reset_मेलनम्[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

ध्रुव संरचना fdt_driver fdt_reset_metanoia = {
	.मेलनम्_सारणी = metanoia_reset_मेलनम्,
	.init	     = metanoia_reset_init,
};
