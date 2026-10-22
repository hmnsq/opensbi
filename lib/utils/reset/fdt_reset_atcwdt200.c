/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_இலக்கம் 0x5aa5
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

நிலையான நிலையற்ற எழுத்து *wdt_addr = வெற்று;

நிலையான முழு ae350_system_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
		திரும்பு 1;
	நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
	இயல்பு:
		திரும்பு 0;
	}
}

நிலையான வெறுமை ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		எனில் (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			செல் fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_இலக்கம், wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

நிலையான கட்டமைப்பு sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

நிலையான முழு atcwdt200_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				மாறிலி கட்டமைப்பு fdt_match *match)
{
	uint64_t reg_addr;
	முழு rc;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, வெற்று);
	எனில் (rc < 0 || !reg_addr)
		திரும்பு SBI_ENODEV;

	wdt_addr = (நிலையற்ற எழுத்து *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
