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

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

संरचना reset_metanoia_data {
	u8 *reg_base;
};

स्थिर संरचना reset_metanoia_data reset_data;

स्थिर पूर्णांक metanoia_system_reset_check(u32 type, u32 reason)
{
	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
		परत 1;
	स्थिती SBI_SRST_RESET_TYPE_SHUTDOWN:
	पूर्वनिर्धारित:
		परत 0;
	}
}

स्थिर शून्य metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

स्थिर संरचना sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

स्थिर पूर्णांक metanoia_reset_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			       स्थिरांक संरचना fdt_match *match)
{
	u64 reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, रिक्त);
	जर (rc < 0 || !reg_addr)
		परत SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

स्थिरांक संरचना fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
