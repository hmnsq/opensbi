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

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/ipi/andes_plicsw.h>

ബാഹ്യ ഘടന plicsw_data plicsw;

പൂർണ്ണസംഖ്യ fdt_plicsw_cold_ipi_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			     സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = fdt_parse_plicsw_കണ്ണി(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_എണ്ണം);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	എങ്കിൽ (rc)
		തിരികെ rc;

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
