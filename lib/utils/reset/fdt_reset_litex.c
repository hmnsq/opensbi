/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

நிலையான நிலையற்ற u32 *litex_soc_base;

நிலையான முழு litex_reset_check(u32 type, u32 reason)
{
	தேர்வு (type) {
	நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
	நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
		திரும்பு 255;
	}

	திரும்பு 0;
}

நிலையான வெறுமை litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

நிலையான கட்டமைப்பு sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

நிலையான முழு litex_reset_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			    மாறிலி கட்டமைப்பு fdt_match *match)
{
	uint64_t reg_addr;
	முழு rc;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, வெற்று);
	எனில் (rc < 0 || !reg_addr)
		திரும்பு SBI_ENODEV;


	litex_soc_base = (நிலையற்ற u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
