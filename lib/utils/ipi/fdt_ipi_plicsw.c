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

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/ipi/andes_plicsw.h>

ಬಾಹ್ಯ ರಚನೆ plicsw_ದತ್ತಾಂಶ plicsw;

ಪೂರ್ಣಾಂಕ fdt_plicsw_cold_ipi_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			     ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = fdt_parse_plicsw_ಗಂಟು(fdt, nodeoff, &plicsw.addr, &plicsw.ಗಾತ್ರ_2,
				   &plicsw.hart_ಎಣಿಕೆ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = plicsw_cold_ipi_init(&plicsw);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ ipi_plicsw_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,plicsw" },
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_ipi_plicsw = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = ipi_plicsw_ಹೊಂದಿಕೆ,
	.init = fdt_plicsw_cold_ipi_init,
};
