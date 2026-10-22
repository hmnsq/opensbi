/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Zong Li <zong@andestech.com>
 *   Nylon Chen <nylon7@andestech.com>
 *   Leo Yu-Chi Liang <ycliang@andestech.com>
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#include <sbi/riscv_io.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/ipi/andes_plicsw.h>

extern struct plicsw_data plicsw;

số_nguyên fdt_plicsw_cold_ipi_init(const void *fdt, số_nguyên nodeoff,
			     const struct fdt_match *match)
{
	số_nguyên rc;

	rc = fdt_parse_plicsw_node(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_count);
	nếu (rc)
		trả_về rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	nếu (rc)
		trả_về rc;

	trả_về 0;
}

static const struct fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

const struct fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
