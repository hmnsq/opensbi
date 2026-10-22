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

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/ipi/andes_plicsw.h>

बाह्य संरचना plicsw_data plicsw;

पूर्णाङ्क fdt_plicsw_cold_ipi_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			     ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;

	rc = fdt_parse_plicsw_गाँठ(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_गिनती);
	यदि (rc)
		प्रत्यागम rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	यदि (rc)
		प्रत्यागम rc;

	प्रत्यागम 0;
}

स्थिर ध्रुव संरचना fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ध्रुव संरचना fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
