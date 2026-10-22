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

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/ipi/andes_plicsw.h>

બાહ્ય સંરચના plicsw_data plicsw;

પૂર્ણાંક fdt_plicsw_cold_ipi_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			     અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;

	rc = fdt_parse_plicsw_ગાંઠ(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_ગણતરી);
	જો (rc)
		પાછા rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	જો (rc)
		પાછા rc;

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

અચલ સંરચના fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
