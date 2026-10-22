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

extern sa_nracanaa plicsw_data plicsw;

puur_naa_nka fdt_plicsw_cold_ipi_init(const void *fdt, puur_naa_nka nodeoff,
			     const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;

	rc = fdt_parse_plicsw_node(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_count);
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	ᤀᤡ᤺ (rc)
		ᤀᤡᤴ rc;

	ᤀᤡᤴ 0;
}

static const sa_nracanaa fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

const sa_nracanaa fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
