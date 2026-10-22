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

#समावेश <sbi/riscv_io.h>
#समावेश <sbi_utils/fdt/fdt_driver.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/ipi/andes_plicsw.h>

बाह्य काष्ठा plicsw_तथ्य plicsw;

पूर्णांक fdt_plicsw_cold_ipi_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			     स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;

	rc = fdt_parse_plicsw_गाँठ(fdt, nodeoff, &plicsw.addr, &plicsw.आकार,
				   &plicsw.hart_गिनती);
	अगर (rc)
		वापस rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	अगर (rc)
		वापस rc;

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

स्थिर काष्ठा fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
