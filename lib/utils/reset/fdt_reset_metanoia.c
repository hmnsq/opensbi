/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Metanoia Communications Inc.
 *
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>

#define SWRSTREQ_CTRL_REG_OFFSET 0x00
#define SWRSTREQ_REG_OFFSET 0x04

સંરચના reset_metanoia_data {
	u8 *reg_base;
};

સ્થિર સંરચના reset_metanoia_data reset_data;

સ્થિર પૂર્ણાંક metanoia_system_reset_check(u32 type, u32 reason)
{
	પસંદગી (type) {
	સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
	સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		પાછા 1;
	સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
	મૂળભૂત:
		પાછા 0;
	}
}

સ્થિર શૂન્ય metanoia_system_reset(u32 type, u32 reason)
{
	writew(0x1, reset_data.reg_base + SWRSTREQ_CTRL_REG_OFFSET);
	writew(0x1, reset_data.reg_base + SWRSTREQ_REG_OFFSET);
}

સ્થિર સંરચના sbi_system_reset_device metanoia_reset = {
	.name		    = "metanoia-mt2824-reboot",
	.system_reset_check = metanoia_system_reset_check,
	.system_reset	    = metanoia_system_reset,
};

સ્થિર પૂર્ણાંક metanoia_reset_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			       અચલ સંરચના fdt_match *match)
{
	u64 reg_addr;
	પૂર્ણાંક rc;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 || !reg_addr)
		પાછા SBI_ENODEV;

	reset_data.reg_base = (u8 *)(ulong)reg_addr;

	sbi_system_reset_add_device(&metanoia_reset);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match metanoia_reset_match[] = {
	{ .compatible = "metanoia,mt2824-reboot" },
	{ /* sentinel */ }
};

અચલ સંરચના fdt_driver fdt_reset_metanoia = {
	.match_table = metanoia_reset_match,
	.init	     = metanoia_reset_init,
};
