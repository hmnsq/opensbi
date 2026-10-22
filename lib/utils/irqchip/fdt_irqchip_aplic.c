/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/fdt_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/aplic.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_aplic_update_idc_map(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
					ರಚನೆ aplic_ದತ್ತಾಂಶ *pd)
{
	ಪೂರ್ಣಾಂಕ i, err, ಎಣಿಕೆ_2, cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;
	u32 phandle, hartid, hartindex;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val || ಎಣಿಕೆ_2 < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 / ಗಾತ್ರ(fdt32_t);

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; i += 2) {
		phandle = fdt32_ಗಮ್ಯ_cpu(val[i]);

		cpu_intc_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(fdt, phandle);
		ಆದರೆ (cpu_intc_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		cpu_ಸ್ಥಾನಾಂತರ = fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(fdt, cpu_intc_ಸ್ಥಾನಾಂತರ);
		ಆದರೆ (cpu_ಸ್ಥಾನಾಂತರ < 0)
			ಮುಂದುವರಿ;

		err = fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid);
		ಆದರೆ (err)
			ಮುಂದುವರಿ;

		hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(hartid);
		ಆದರೆ (hartindex == -1U)
			ಮುಂದುವರಿ;

		pd->idc_map[i / 2] = hartindex;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_aplic_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ aplic_ದತ್ತಾಂಶ *pd;

	pd = sbi_zalloc(ಗಾತ್ರ(*pd));
	ಆದರೆ (!pd)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_parse_aplic_ಗಂಟು(fdt, nodeoff, pd);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	ಆದರೆ (pd->ಅಂಕೆ_idc) {
		pd->idc_map = sbi_zalloc(ಗಾತ್ರ(*pd->idc_map) * pd->ಅಂಕೆ_idc);
		ಆದರೆ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			ಹೋಗು fail_free_ದತ್ತಾಂಶ;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ಆದರೆ (rc)
			ಹೋಗು fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_idc_map;

	ಹಿಂತಿರುಗಿ 0;

fail_free_idc_map:
	ಆದರೆ (pd->ಅಂಕೆ_idc)
		sbi_free(pd->idc_map);
fail_free_ದತ್ತಾಂಶ:
	sbi_free(pd);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ irqchip_aplic_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_irqchip_aplic = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = irqchip_aplic_ಹೊಂದಿಕೆ,
	.init = irqchip_aplic_cold_init,
};
