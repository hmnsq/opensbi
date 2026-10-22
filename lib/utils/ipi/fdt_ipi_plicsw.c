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

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/ipi/andes_plicsw.h>

வெளி கட்டமைப்பு plicsw_data plicsw;

முழு fdt_plicsw_cold_ipi_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			     மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;

	rc = fdt_parse_plicsw_முனை(fdt, nodeoff, &plicsw.addr, &plicsw.size,
				   &plicsw.hart_மொத்தம்);
	எனில் (rc)
		திரும்பு rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	எனில் (rc)
		திரும்பு rc;

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match ipi_plicsw_match[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_ipi_plicsw = {
	.match_table = ipi_plicsw_match,
	.init = fdt_plicsw_cold_ipi_init,
};
