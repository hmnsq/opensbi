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

#परिभाषय ATCWDT200_WP_अङ्कः 0x5aa5
#परिभाषय WREN_REG 0x18
#परिभाषय CTRL_REG 0x10
#परिभाषय RST_TIME_OFF 8
#परिभाषय RST_TIME_MSK (0x3 << RST_TIME_OFF)
#परिभाषय RST_CLK_128 (0 << RST_TIME_OFF)
#परिभाषय RST_CLK_256 (1 << RST_TIME_OFF)
#परिभाषय RST_CLK_512 (2 << RST_TIME_OFF)
#परिभाषय RST_CLK_1024 (3 << RST_TIME_OFF)
#परिभाषय INT_TIME_OFF 4
#परिभाषय INT_TIME_MSK (0xf << INT_TIME_OFF)
#परिभाषय INT_CLK_64 (0 << INT_TIME_OFF)
#परिभाषय INT_CLK_256 (1 << INT_TIME_OFF)
#परिभाषय INT_CLK_1024 (2 << INT_TIME_OFF)
#परिभाषय INT_CLK_2048 (3 << INT_TIME_OFF)
#परिभाषय INT_CLK_4096 (4 << INT_TIME_OFF)
#परिभाषय INT_CLK_8192 (5 << INT_TIME_OFF)
#परिभाषय INT_CLK_16384 (6 << INT_TIME_OFF)
#परिभाषय INT_CLK_32768 (7 << INT_TIME_OFF)
#परिभाषय RST_EN (1 << 3)
#परिभाषय INT_EN (1 << 2)
#परिभाषय CLK_PCLK (1 << 1)
#परिभाषय WDT_EN (1 << 0)

#परिभाषय AE350_FLASH_आधारः 0x80000000

स्थिर चञ्चल वर्ण *wdt_addr = रिक्त;

स्थिर पूर्णाङ्क ae350_system_reset_check(u32 type, u32 reason)
{
	विकल्प (type) {
	पक्ष SBI_SRST_RESET_TYPE_COLD_REBOOT:
		प्रत्यागम 1;
	पक्ष SBI_SRST_RESET_TYPE_SHUTDOWN:
	पक्ष SBI_SRST_RESET_TYPE_WARM_REBOOT:
	स्वभाव:
		प्रत्यागम 0;
	}
}

स्थिर शून्य ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		यदि (atcsmu_समुच्चयः_reset_सदिशम्(AE350_FLASH_आधारः, i))
			गच्छ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_अङ्कः, wdt_addr + WREN_REG);
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

स्थिर पूर्णाङ्क atcwdt200_reset_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	uint64_t reg_addr;
	पूर्णाङ्क rc;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, रिक्त);
	यदि (rc < 0 || !reg_addr)
		प्रत्यागम SBI_ENODEV;

	wdt_addr = (चञ्चल वर्ण *)(अचिह्नित दीर्घ)reg_addr;
	sbi_system_reset_सङ्कलय_device(&atcwdt200_reset);

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_मेलनम् atcwdt200_reset_मेलनम्[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ध्रुव संरचना fdt_driver fdt_reset_atcwdt200 = {
	.मेलनम्_सारणी = atcwdt200_reset_मेलनम्,
	.init	     = atcwdt200_reset_init,
};
