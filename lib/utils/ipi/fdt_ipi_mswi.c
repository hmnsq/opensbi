/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_driver.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/ipi/aclint_mswi.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ipi_mswi_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಾನಾಂತರ;
	ರಚನೆ aclint_mswi_ದತ್ತಾಂಶ *ms;

	ms = sbi_zalloc(ಗಾತ್ರ(*ms));
	ಆದರೆ (!ms)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_parse_aclint_ಗಂಟು(fdt, nodeoff, false, false,
				   &ms->addr, &ms->ಗಾತ್ರ_2, ಖಾಲಿ, ಖಾಲಿ,
				   &ms->ಮೊದಲ_hartid, &ms->hart_ಎಣಿಕೆ);
	ಆದರೆ (rc) {
		sbi_free(ms);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ (ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ) {
		/* Adjust MSWI address and size for CLINT device */
		ಸ್ಥಾನಾಂತರ = *((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *)ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ);
		ms->addr += ಸ್ಥಾನಾಂತರ;
		ಆದರೆ ((ms->ಗಾತ್ರ_2 - ಸ್ಥಾನಾಂತರ) < ACLINT_MSWI_ಗಾತ್ರ)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ms->ಗಾತ್ರ_2 = ACLINT_MSWI_ಗಾತ್ರ;
	}

	rc = aclint_mswi_cold_init(ms);
	ಆದರೆ (rc) {
		sbi_free(ms);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ clint_ಸ್ಥಾನಾಂತರ = CLINT_MSWI_ಸ್ಥಾನಾಂತರ;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ ipi_mswi_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,clint0", .ದತ್ತಾಂಶ = &clint_ಸ್ಥಾನಾಂತರ },
	{ .compatible = "sifive,clint0", .ದತ್ತಾಂಶ = &clint_ಸ್ಥಾನಾಂತರ },
	{ .compatible = "thead,c900-clint", .ದತ್ತಾಂಶ = &clint_ಸ್ಥಾನಾಂತರ },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_ipi_mswi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = ipi_mswi_ಹೊಂದಿಕೆ,
	.init = ipi_mswi_cold_init,
};
