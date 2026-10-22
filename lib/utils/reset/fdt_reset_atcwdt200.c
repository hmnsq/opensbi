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

#تعریف_کرو ATCWDT200_WP_عدد 0x5aa5
#تعریف_کرو WREN_REG 0x18
#تعریف_کرو CTRL_REG 0x10
#تعریف_کرو RST_وقت_OFF 8
#تعریف_کرو RST_وقت_MSK (0x3 << RST_وقت_OFF)
#تعریف_کرو RST_CLK_128 (0 << RST_وقت_OFF)
#تعریف_کرو RST_CLK_256 (1 << RST_وقت_OFF)
#تعریف_کرو RST_CLK_512 (2 << RST_وقت_OFF)
#تعریف_کرو RST_CLK_1024 (3 << RST_وقت_OFF)
#تعریف_کرو INT_وقت_OFF 4
#تعریف_کرو INT_وقت_MSK (0xf << INT_وقت_OFF)
#تعریف_کرو INT_CLK_64 (0 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_256 (1 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_1024 (2 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_2048 (3 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_4096 (4 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_8192 (5 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_16384 (6 << INT_وقت_OFF)
#تعریف_کرو INT_CLK_32768 (7 << INT_وقت_OFF)
#تعریف_کرو RST_EN (1 << 3)
#تعریف_کرو INT_EN (1 << 2)
#تعریف_کرو CLK_PCLK (1 << 1)
#تعریف_کرو WDT_EN (1 << 0)

#تعریف_کرو AE350_FLASH_بنیاد 0x80000000

ساکن متلون حرف *wdt_addr = تہی;

ساکن صحیح ae350_system_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		واپس 1;
	حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
	طےشدہ:
		واپس 0;
	}
}

ساکن خالی ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		اگر (atcsmu_سیٹ_reset_سمتیہ(AE350_FLASH_بنیاد, i))
			جاؤ fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_عدد, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ساکن ساخت sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ساکن صحیح atcwdt200_reset_init(ثابت خالی *fdt, صحیح nodeoff,
				ثابت ساخت fdt_مطابقت *مطابقت)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 || !reg_addr)
		واپس SBI_ENODEV;

	wdt_addr = (متلون حرف *)(غیرعلامتی طویل)reg_addr;
	sbi_system_reset_جوڑو_device(&atcwdt200_reset);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت atcwdt200_reset_مطابقت[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ثابت ساخت fdt_driver fdt_reset_atcwdt200 = {
	.مطابقت_جدول = atcwdt200_reset_مطابقت,
	.init	     = atcwdt200_reset_init,
};
