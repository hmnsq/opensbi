/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_अंक 0x5aa5
#define WREN_REG 0x18
#define CTRL_REG 0x10
#define RST_TIME_OFF 8
#define RST_TIME_MSK (0x3 << RST_TIME_OFF)
#define RST_CLK_128 (0 << RST_TIME_OFF)
#define RST_CLK_256 (1 << RST_TIME_OFF)
#define RST_CLK_512 (2 << RST_TIME_OFF)
#define RST_CLK_1024 (3 << RST_TIME_OFF)
#define INT_TIME_OFF 4
#define INT_TIME_MSK (0xf << INT_TIME_OFF)
#define INT_CLK_64 (0 << INT_TIME_OFF)
#define INT_CLK_256 (1 << INT_TIME_OFF)
#define INT_CLK_1024 (2 << INT_TIME_OFF)
#define INT_CLK_2048 (3 << INT_TIME_OFF)
#define INT_CLK_4096 (4 << INT_TIME_OFF)
#define INT_CLK_8192 (5 << INT_TIME_OFF)
#define INT_CLK_16384 (6 << INT_TIME_OFF)
#define INT_CLK_32768 (7 << INT_TIME_OFF)
#define RST_EN (1 << 3)
#define INT_EN (1 << 2)
#define CLK_PCLK (1 << 1)
#define WDT_EN (1 << 0)

#define AE350_FLASH_BASE 0x80000000

स्थिर अस्थिर अक्षर *wdt_addr = रिक्त;

स्थिर पूर्णांक ae350_system_reset_check(u32 type, u32 reason)
{
	निवड (type) {
	स्थिती SBI_SRST_RESET_TYPE_COLD_REBOOT:
		परत 1;
	स्थिती SBI_SRST_RESET_TYPE_SHUTDOWN:
	स्थिती SBI_SRST_RESET_TYPE_WARM_REBOOT:
	पूर्वनिर्धारित:
		परत 0;
	}
}

स्थिर शून्य ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		जर (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			जा fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_अंक, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

स्थिर संरचना sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

स्थिर पूर्णांक atcwdt200_reset_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				स्थिरांक संरचना fdt_match *match)
{
	uint64_t reg_addr;
	पूर्णांक rc;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, रिक्त);
	जर (rc < 0 || !reg_addr)
		परत SBI_ENODEV;

	wdt_addr = (अस्थिर अक्षर *)(अचिन्हित दीर्घ)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	परत 0;
}

स्थिर स्थिरांक संरचना fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

स्थिरांक संरचना fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
