/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

स्थिर अस्थिर u32 *litex_soc_base;

स्थिर पूर्णांक litex_reset_check(u32 type, u32 reason)
{
	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
		परत 255;
	}

	परत 0;
}

स्थिर शून्य litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

स्थिर संरचना sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

स्थिर पूर्णांक litex_reset_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			    स्थिरांक संरचना fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, रिक्त);
	जर (rc < 0 || !reg_addr)
		परत SBI_ENODEV;


	litex_soc_base = (अस्थिर u32 *)(अचिन्हित दीर्घ)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
