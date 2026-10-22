/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/irqchip/fdt_irqchip.h>
#અન્તર્ભાવ <sbi_utils/irqchip/aplic.h>

સ્થિર પૂર્ણાંક irqchip_aplic_update_idc_map(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
					સંરચના aplic_data *pd)
{
	પૂર્ણાંક i, err, ગણતરી, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	અચલ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ગણતરી);
	જો (!val || ગણતરી < કદ(fdt32_t))
		પાછા SBI_EINVAL;
	ગણતરી = ગણતરી / કદ(fdt32_t);

	માટે (i = 0; i < ગણતરી; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
		જો (cpu_intc_offset < 0)
			ચાલુ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		જો (cpu_offset < 0)
			ચાલુ;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (err)
			ચાલુ;

		hartindex = sbi_hartid_to_hartindex(hartid);
		જો (hartindex == -1U)
			ચાલુ;

		pd->idc_map[i / 2] = hartindex;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક irqchip_aplic_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના aplic_data *pd;

	pd = sbi_zalloc(કદ(*pd));
	જો (!pd)
		પાછા SBI_ENOMEM;

	rc = fdt_parse_aplic_ગાંઠ(fdt, nodeoff, pd);
	જો (rc)
		જાઓ fail_free_data;

	જો (pd->અંક_idc) {
		pd->idc_map = sbi_zalloc(કદ(*pd->idc_map) * pd->અંક_idc);
		જો (!pd->idc_map) {
			rc = SBI_ENOMEM;
			જાઓ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		જો (rc)
			જાઓ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	જો (rc)
		જાઓ fail_free_idc_map;

	પાછા 0;

fail_free_idc_map:
	જો (pd->અંક_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

અચલ સંરચના fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
