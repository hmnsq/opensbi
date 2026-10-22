/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Andes Technology Corporation
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/hsm/fdt_hsm_andes_atcsmu.h>

#تعریف_کرو ATCWDT200_WP_عدد 0x5aa5
#تعریف_کرو WREN_REG 0x18
#تعریف_کرو CTRL_REG 0x10
#تعریف_کرو RST_ویلا_OFF 8
#تعریف_کرو RST_ویلا_MSK (0x3 << RST_ویلا_OFF)
#تعریف_کرو RST_CLK_128 (0 << RST_ویلا_OFF)
#تعریف_کرو RST_CLK_256 (1 << RST_ویلا_OFF)
#تعریف_کرو RST_CLK_512 (2 << RST_ویلا_OFF)
#تعریف_کرو RST_CLK_1024 (3 << RST_ویلا_OFF)
#تعریف_کرو INT_ویلا_OFF 4
#تعریف_کرو INT_ویلا_MSK (0xf << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_64 (0 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_256 (1 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_1024 (2 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_2048 (3 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_4096 (4 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_8192 (5 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_16384 (6 << INT_ویلا_OFF)
#تعریف_کرو INT_CLK_32768 (7 << INT_ویلا_OFF)
#تعریف_کرو RST_EN (1 << 3)
#تعریف_کرو INT_EN (1 << 2)
#تعریف_کرو CLK_PCLK (1 << 1)
#تعریف_کرو WDT_EN (1 << 0)

#تعریف_کرو AE350_FLASH_ادھار 0x80000000

ساكن متطاير حرف *wdt_addr = عدم;

ساكن صحيح ae350_system_reset_check(u32 type, u32 reason)
{
	بدل (type) {
	حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
		أرجع 1;
	حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
	حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
	افتراضي:
		أرجع 0;
	}
}

ساكن فراغ ae350_system_reset(u32 type, u32 reason)
{
	sbi_for_each_hartindex(i)
		إذا (atcsmu_سیٹ_reset_سمتیہ(AE350_FLASH_ادھار, i))
			اذهب fail;

	/* Program WDT control register  */
	writew(ATCWDT200_WP_عدد, wdt_addr + WREN_REG);
	writel(INT_CLK_32768 | INT_EN | RST_CLK_128 | RST_EN | WDT_EN,
	       wdt_addr + CTRL_REG);

fail:
	sbi_hart_hang();
}

ساكن بنية sbi_system_reset_device atcwdt200_reset = {
	.name		    = "atcwdt200",
	.system_reset_check = ae350_system_reset_check,
	.system_reset	    = ae350_system_reset,
};

ساكن صحيح atcwdt200_reset_init(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت بنية fdt_میل *میل)
{
	uint64_t reg_addr;
	صحيح rc;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, عدم);
	إذا (rc < 0 || !reg_addr)
		أرجع SBI_ENODEV;

	wdt_addr = (متطاير حرف *)(بنانشانی لما)reg_addr;
	sbi_system_reset_جوڑو_device(&atcwdt200_reset);

	أرجع 0;
}

ساكن ثابت بنية fdt_میل atcwdt200_reset_میل[] = {
	{ .compatible = "andestech,atcwdt200" },
	{},
};

ثابت بنية fdt_driver fdt_reset_atcwdt200 = {
	.میل_جدول = atcwdt200_reset_میل,
	.init	     = atcwdt200_reset_init,
};
