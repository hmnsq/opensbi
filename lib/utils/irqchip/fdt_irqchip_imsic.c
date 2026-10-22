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
#ಅನ್ತರ್ಭಾವ <sbi_utils/irqchip/imsic.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_imsic_update_hartid_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
					     ರಚನೆ imsic_ದತ್ತಾಂಶ *id)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	ಪೂರ್ಣಾಂಕ i, err, ಎಣಿಕೆ_2, cpu_ಸ್ಥಾನಾಂತರ, cpu_intc_ಸ್ಥಾನಾಂತರ;

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
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಆಯ್ಕೆ (hwirq) {
		ಸ್ಥಿತಿ IRQ_M_EXT:
			err = imsic_map_hartid_ಗಮ್ಯ_ದತ್ತಾಂಶ(hartid, id, i / 2);
			ಆದರೆ (err)
				ಹಿಂತಿರುಗಿ err;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ irqchip_imsic_cold_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				   ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ imsic_ದತ್ತಾಂಶ *id;

	id = sbi_zalloc(ಗಾತ್ರ(*id));
	ಆದರೆ (!id)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	rc = fdt_parse_imsic_ಗಂಟು(fdt, nodeoff, id);
	ಆದರೆ (rc || !id->targets_mmode)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	rc = imsic_cold_irqchip_init(id);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	rc = irqchip_imsic_update_hartid_ಕೋಷ್ಟಕ(fdt, nodeoff, id);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_ದತ್ತಾಂಶ;

	ಹಿಂತಿರುಗಿ 0;

fail_free_ದತ್ತಾಂಶ:
	sbi_free(id);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ irqchip_imsic_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_irqchip_imsic = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = irqchip_imsic_ಹೊಂದಿಕೆ,
	.init = irqchip_imsic_cold_init,
};
