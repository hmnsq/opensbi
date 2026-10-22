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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_imsic_update_hartid_table(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
					     ಸಂರಚನಾ imsic_data *id)
{
	ಧ್ರುವ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	ಪೂರ್ಣಾಙ್ಕ i, err, ಎಣಿಕೆ, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ಎಣಿಕೆ);
	ಯದಿ (!val || ಎಣಿಕೆ < ಪರಿಮಾಣ(fdt32_t))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಎಣಿಕೆ = ಎಣಿಕೆ / ಪರಿಮಾಣ(fdt32_t);

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_ಗಂಟು_offset_by_phandle(fdt, phandle);
		ಯದಿ (cpu_intc_offset < 0)
			ಅನುವರ್ತ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ಯದಿ (cpu_offset < 0)
			ಅನುವರ್ತ;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ಯದಿ (err)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		ವಿಕಲ್ಪ (hwirq) {
		ಪಕ್ಷ IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			ಯದಿ (err)
				ಪ್ರತ್ಯಾಗಮ err;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_imsic_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				   ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ imsic_data *id;

	id = sbi_zalloc(ಪರಿಮಾಣ(*id));
	ಯದಿ (!id)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_parse_imsic_ಗಂಟು(fdt, nodeoff, id);
	ಯದಿ (rc || !id->targets_mmode)
		ಗಚ್ಛ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_data:
	sbi_free(id);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
