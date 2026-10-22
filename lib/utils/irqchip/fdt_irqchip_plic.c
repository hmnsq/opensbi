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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_plic_update_context_map(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
					   ಸಂರಚನಾ plic_data *pd)
{
	ಧ್ರುವ fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	ಪೂರ್ಣಾಙ್ಕ i, err, ಎಣಿಕೆ, cpu_offset, cpu_intc_offset;

	ಪ್ರತಿ (i = 0; i < sbi_hart_ಎಣಿಕೆ(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			ಅನುವರ್ತ;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ಯದಿ (hartindex == -1U)
			ಅನುವರ್ತ;

		ವಿಕಲ್ಪ (hwirq) {
		ಪಕ್ಷ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ಭಙ್ಗ;
		ಪಕ್ಷ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ irqchip_plic_cold_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				  ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_ಎಣಿಕೆ()));
	ಯದಿ (!pd)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	rc = fdt_parse_plic_ಗಂಟು(fdt, nodeoff, pd);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_data;

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_data:
	sbi_free(pd);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (ಶೂನ್ಯ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
