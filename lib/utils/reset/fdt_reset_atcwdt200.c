/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#define ATCWDT200_WP_رقم 0x5aa5
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

ایستا فرار نویسه *wdt_addr = پوچ;

ایستا صحیح ae350_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		بازگشت 1;
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
	پیشفرض:
		بازگشت 0;
	}
}

ایستا تهی ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		اگر (atcsmu_set_reset_vector(AE350_FLASH_BASE, i))
			برو fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_رقم, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ایستا ساختار sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ایستا صحیح atcwdt200_reset_init(ثابت تهی *fdt, صحیح nodeoff,
				ثابت ساختار fdt_match *match)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, پوچ);
	اگر (rc < 0 || !reg_addr)
		بازگشت SBI_ENODEV;

	wdt_addr = (فرار نویسه *)(unsigned long)reg_addr;
	sbi_system_reset_add_device(&atcwdt200_reset);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match atcwdt200_reset_match[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ثابت ساختار fdt_driver fdt_reset_atcwdt200 = {
	.match_table = atcwdt200_reset_match,
	.init	     = atcwdt200_reset_init,
};
