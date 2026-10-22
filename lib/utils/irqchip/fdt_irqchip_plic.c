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

স্থির পূর্ণসংখ্যা irqchip_plic_update_context_map(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
					   কাঠামো plic_তথ্য *pd)
{
	ধ্রুবক fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	পূর্ণসংখ্যা i, err, গুনতি, cpu_offset, cpu_intc_offset;

	জন্য (i = 0; i < sbi_hart_গুনতি(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < আকার(fdt32_t))
		ফেরত SBI_EINVAL;
	গুনতি = গুনতি / আকার(fdt32_t);

	জন্য (i = 0; i < গুনতি; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			চালু;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			চালু;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			চালু;

		hartindex = sbi_hartid_to_hartindex(hartid);
		যদি (hartindex == -1U)
			চালু;

		নির্বাচন (hwirq) {
		ক্ষেত্র IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			বিরতি;
		ক্ষেত্র IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			বিরতি;
		}
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা irqchip_plic_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				  ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো plic_তথ্য *pd;

	pd = sbi_zalloc(PLIC_তথ্য_আকার(sbi_hart_গুনতি()));
	যদি (!pd)
		ফেরত SBI_ENOMEM;

	rc = fdt_parse_plic_গ্রন্থি(fdt, nodeoff, pd);
	যদি (rc)
		যাও fail_free_তথ্য;

	pd->flags = (unsigned long)match->তথ্য;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	যদি (rc)
		যাও fail_free_তথ্য;

	rc = plic_cold_irqchip_init(pd);
	যদি (rc)
		যাও fail_free_তথ্য;

	ফেরত 0;

fail_free_তথ্য:
	sbi_free(pd);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .তথ্য = (শূন্য *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ধ্রুবক কাঠামো fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
