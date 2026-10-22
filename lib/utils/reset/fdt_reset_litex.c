/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

#تعریف_کرو RESET_CTRL			0x0

ساکن متلون u32 *litex_soc_بنیاد;

ساکن صحیح litex_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		واپس 255;
	}

	واپس 0;
}

ساکن خالی litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_بنیاد + RESET_CTRL);
}

ساکن ساخت sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ساکن صحیح litex_reset_init(ثابت خالی *fdt, صحیح nodeoff,
			    ثابت ساخت fdt_مطابقت *مطابقت)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, تہی);
	اگر (rc < 0 || !reg_addr)
		واپس SBI_ENODEV;


	litex_soc_بنیاد = (متلون u32 *)(غیرعلامتی طویل)reg_addr;

	sbi_system_reset_جوڑو_device(&litex_reset);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت litex_reset_مطابقت[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ثابت ساخت fdt_driver fdt_reset_litex = {
	.مطابقت_جدول = litex_reset_مطابقت,
	.init = litex_reset_init,
};
