/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

સ્થિર અસ્થિર u32 *litex_soc_base;

સ્થિર પૂર્ણાંક litex_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
		પાછા 255;
	}

	પાછા 0;
}

સ્થિર શૂન્ય litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

સ્થિર સંરચના sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

સ્થિર પૂર્ણાંક litex_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			    અચલ સંરચના fdt_match *match)
{
	uint64_t reg_addr;
	પૂર્ણાંક rc;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 || !reg_addr)
		પાછા SBI_ENODEV;


	litex_soc_base = (અસ્થિર u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

અચલ સંરચના fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
