/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESET_CTRL			0x0

ਸਥਿਰ ਅਸਥਿਰ u32 *litex_soc_ਅਧਾਰ;

ਸਥਿਰ ਪੂਰਨ litex_reset_check(u32 type, u32 reason)
{
	ਚੋਣ (type) {
	ਹਾਲਤ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	ਹਾਲਤ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ਵਾਪਸ 255;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_ਅਧਾਰ + RESET_CTRL);
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

ਸਥਿਰ ਪੂਰਨ litex_reset_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			    ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	uint64_t reg_addr;
	ਪੂਰਨ rc;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, ਨਿਰਾ);
	ਜੇ (rc < 0 || !reg_addr)
		ਵਾਪਸ SBI_ENODEV;


	litex_soc_ਅਧਾਰ = (ਅਸਥਿਰ u32 *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr;

	sbi_system_reset_ਜੋੜੋ_device(&litex_reset);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ litex_reset_ਮੇਲ[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_reset_litex = {
	.ਮੇਲ_ਸਾਰਣੀ = litex_reset_ਮੇਲ,
	.init = litex_reset_init,
};
