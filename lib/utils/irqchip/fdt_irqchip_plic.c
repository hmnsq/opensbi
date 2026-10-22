/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/irqchip/fdt_irqchip.h>
#અન્તર્ભાવ <sbi_utils/irqchip/plic.h>

સ્થિર પૂર્ણાંક irqchip_plic_update_context_map(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
					   સંરચના plic_data *pd)
{
	અચલ fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	પૂર્ણાંક i, err, ગણતરી, cpu_offset, cpu_intc_offset;

	માટે (i = 0; i < sbi_hart_ગણતરી(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			ચાલુ;

		hartindex = sbi_hartid_to_hartindex(hartid);
		જો (hartindex == -1U)
			ચાલુ;

		પસંદગી (hwirq) {
		સ્થિતિ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			તોડો;
		સ્થિતિ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			તોડો;
		}
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક irqchip_plic_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				  અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	સંરચના plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_ગણતરી()));
	જો (!pd)
		પાછા SBI_ENOMEM;

	rc = fdt_parse_plic_ગાંઠ(fdt, nodeoff, pd);
	જો (rc)
		જાઓ fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	જો (rc)
		જાઓ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	જો (rc)
		જાઓ fail_free_data;

	પાછા 0;

fail_free_data:
	sbi_free(pd);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (શૂન્ય *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

અચલ સંરચના fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
