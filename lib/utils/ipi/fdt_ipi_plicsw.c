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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_driver.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/ipi/andes_plicsw.h>

ਬਾਹਰੀ ਬਣਤਰ plicsw_ਅੰਕੜੇ plicsw;

ਪੂਰਨ fdt_plicsw_cold_ipi_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			     ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;

	rc = fdt_parse_plicsw_ਗੰਢ(fdt, nodeoff, &plicsw.addr, &plicsw.ਮਾਪ,
				   &plicsw.hart_ਗਿਣਤੀ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ ipi_plicsw_ਮੇਲ[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_ipi_plicsw = {
	.ਮੇਲ_ਸਾਰਣੀ = ipi_plicsw_ਮੇਲ,
	.init = fdt_plicsw_cold_ipi_init,
};
