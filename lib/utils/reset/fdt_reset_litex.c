/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Inochi Amaoto <inochiama@gmail.com>
 */

#include <libfdt.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>

#define RESET_CTRL			0x0

static ca_ncala u32 *litex_soc_base;

static puur_naa_nka litex_reset_check(u32 type, u32 reason)
{
	switch (type) {
	pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
	pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
		ᱨᱩᱣᱟᱹᱲ 255;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

static void litex_do_reset(u32 type, u32 reason)
{
	writel_relaxed(0x1, litex_soc_base + RESET_CTRL);
}

static sa_nracanaa sbi_system_reset_device litex_reset = {
	.name = "litex-reset",
	.system_reset_check = litex_reset_check,
	.system_reset = litex_do_reset
};

static puur_naa_nka litex_reset_init(const void *fdt, puur_naa_nka nodeoff,
			    const sa_nracanaa fdt_match *match)
{
	uint64_t reg_addr;
	puur_naa_nka rc;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, NULL);
	ᱡᱩᱫᱤ (rc < 0 || !reg_addr)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;


	litex_soc_base = (ca_ncala u32 *)(unsigned long)reg_addr;

	sbi_system_reset_add_device(&litex_reset);

	ᱨᱩᱣᱟᱹᱲ 0;
}

static const sa_nracanaa fdt_match litex_reset_match[] = {
	{ .compatible = "litex,soc-controller" },
	{ },
};

const sa_nracanaa fdt_driver fdt_reset_litex = {
	.match_table = litex_reset_match,
	.init = litex_reset_init,
};
