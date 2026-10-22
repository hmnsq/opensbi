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
#અન્તર્ભાવ <sbi_utils/irqchip/imsic.h>

સ્થિર પૂર્ણાંક irqchip_imsic_update_hartid_table(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
					     સંરચના imsic_data *id)
{
	અચલ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	પૂર્ણાંક i, err, ગણતરી, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ગણતરી);
	જો (!val || ગણતરી < કદ(fdt32_t))
		પાછા SBI_EINVAL;
	ગણતરી = ગણતરી / કદ(fdt32_t);

	માટે (i = 0; i < ગણતરી; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);
		જો (cpu_intc_offset < 0)
			ચાલુ;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		જો (cpu_offset < 0)
			ચાલુ;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		જો (err)
			પાછા SBI_EINVAL;

		પસંદગી (hwirq) {
		સ્થિતિ IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			જો (err)
				પાછા err;
			તોડો;
		મૂળભૂત:
			તોડો;
		}
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક irqchip_imsic_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				   અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના imsic_data *id;

	id = sbi_zalloc(કદ(*id));
	જો (!id)
		પાછા SBI_ENOMEM;

	rc = fdt_parse_imsic_ગાંઠ(fdt, nodeoff, id);
	જો (rc || !id->targets_mmode)
		જાઓ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	જો (rc)
		જાઓ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	જો (rc)
		જાઓ fail_free_data;

	પાછા 0;

fail_free_data:
	sbi_free(id);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

અચલ સંરચના fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
