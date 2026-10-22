/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/irqchip/fdt_irqchip.h>
#অন্তর্ভাব <sbi_utils/irqchip/plic.h>

স্থির পূর্ণাঙ্ক irqchip_plic_update_context_map(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
					   সংরচনা plic_data *pd)
{
	ধ্রুব fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	পূর্ণাঙ্ক i, err, গুনতি, cpu_offset, cpu_intc_offset;

	প্রতি (i = 0; i < sbi_hart_গুনতি(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		প্রত্যাগম SBI_EINVAL;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	প্রতি (i = 0; i < গুনতি; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুবর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুবর্ত;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			অনুবর্ত;

		hartindex = sbi_hartid_to_hartindex(hartid);
		যদি (hartindex == -1U)
			অনুবর্ত;

		বিকল্প (hwirq) {
		পক্ষ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ভঙ্গ;
		পক্ষ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ভঙ্গ;
		}
	}

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক irqchip_plic_cold_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				  ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_গুনতি()));
	যদি (!pd)
		প্রত্যাগম SBI_ENOMEM;

	rc = fdt_parse_plic_গ্রন্থি(fdt, nodeoff, pd);
	যদি (rc)
		গচ্ছ fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	যদি (rc)
		গচ্ছ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	যদি (rc)
		গচ্ছ fail_free_data;

	প্রত্যাগম 0;

fail_free_data:
	sbi_free(pd);
	প্রত্যাগম rc;
}

স্থির ধ্রুব সংরচনা fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (শূন্য *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ধ্রুব সংরচনা fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
