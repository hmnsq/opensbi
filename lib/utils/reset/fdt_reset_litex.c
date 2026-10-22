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

#define RESET_CTRL			0x0

ایستا فرار u32 *litex_soc_base;

ایستا صحیح litex_reset_check(u32 type, u32 reason)
{
	انتخاب (type) {
	حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
	حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
		بازگشت 255;
	}

	بازگشت 0;
}

ایستا تهی litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

ایستا ساختار sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ایستا صحیح litex_reset_init(ثابت تهی *fdt, صحیح nodeoff,
			    ثابت ساختار fdt_match *match)
{
	uint64_t reg_addr;
	صحیح rc;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, پوچ);
	اگر (rc < 0 || !reg_addr)
		بازگشت SBI_ENODEV;


	litex_soc_base = (فرار u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	بازگشت 0;
}

ایستا ثابت ساختار fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ثابت ساختار fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
