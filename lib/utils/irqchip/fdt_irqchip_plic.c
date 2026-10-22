/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/fdt_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/plic.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_plic_update_context_map(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
					   ರಚನೆ plic_ದತ್ತಾಂಶ *pd)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	ಪೂರ್ಣಾಂಕ i, err, ಎಣಿಕೆ_2, cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;

	ಗಾಗಿ (i = 0; i < sbi_hart_ಎಣಿಕೆ(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ಎಣಿಕೆ_2);
	ಆದರೆ (!val || ಎಣಿಕೆ_2 < ಗಾತ್ರ(fdt32_t))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 / ಗಾತ್ರ(fdt32_t);

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; i += 2) {
		phandle = fdt32_ಗಮ್ಯ_cpu(val[i]);
		hwirq = fdt32_ಗಮ್ಯ_cpu(val[i + 1]);

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

		ಆಯ್ಕೆ (hwirq) {
		ಸ್ಥಿತಿ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_plic_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				  ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ plic_ದತ್ತಾಂಶ *pd;

	pd = sbi_zalloc(PLIC_ದತ್ತಾಂಶ_ಗಾತ್ರ(sbi_hart_ಎಣಿಕೆ()));
	ಆದರೆ (!pd)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_parse_plic_ಗಂಟು(fdt, nodeoff, pd);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	pd->flags = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	rc = plic_cold_irqchip_init(pd);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	ಹಿಂತಿರುಗಿ 0;

fail_free_ದತ್ತಾಂಶ:
	sbi_free(pd);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ irqchip_plic_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .ದತ್ತಾಂಶ = (ಶೂನ್ಯ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_irqchip_plic = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = irqchip_plic_ಹೊಂದಿಕೆ,
	.init = irqchip_plic_cold_init,
};
