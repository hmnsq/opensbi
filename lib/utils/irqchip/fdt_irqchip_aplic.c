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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_aplic_update_idc_map(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
					ಸಂರಚನಾ aplic_data *pd)
{
	ಪೂರ್ಣಾಙ್ಕ i, err, ಎಣಿಕೆ, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ಧ್ರುವ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ಎಣಿಕೆ);
	ಯದಿ (!val || ಎಣಿಕೆ < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಎಣಿಕೆ = ಎಣಿಕೆ / ಪರಿಮಾಣ(fdt32_t);

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
		ಯದಿ (cpu_intc_offset < 0)
			ಅನುವರ್ತ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ಯದಿ (cpu_offset < 0)
			ಅನುವರ್ತ;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (err)
			ಅನುವರ್ತ;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ಯದಿ (hartindex == -1U)
			ಅನುವರ್ತ;

		pd->idc_map[i / 2] = hartindex;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_aplic_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ aplic_data *pd;

	pd = sbi_zalloc(ಪರಿಮಾಣ(*pd));
	ಯದಿ (!pd)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_parse_aplic_ಗಂಟು(fdt, nodeoff, pd);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	ಯದಿ (pd->ಅಂಕೆ_idc) {
		pd->idc_map = sbi_zalloc(ಪರಿಮಾಣ(*pd->idc_map) * pd->ಅಂಕೆ_idc);
		ಯದಿ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			ಗಚ್ಛ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ಯದಿ (rc)
			ಗಚ್ಛ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_idc_map;

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_idc_map:
	ಯದಿ (pd->ಅಂಕೆ_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
