/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/irqchip/fdt_irqchip.h>
#অন্তর্ভাব <sbi_utils/irqchip/aplic.h>

স্থির পূর্ণসংখ্যা irqchip_aplic_update_idc_map(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
					কাঠামো aplic_তথ্য *pd)
{
	পূর্ণসংখ্যা i, err, গুনতি, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ধ্রুবক fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < আকার(fdt32_t))
		ফেরত SBI_EINVAL;
	গুনতি = গুনতি / আকার(fdt32_t);

	জন্য (i = 0; i < গুনতি; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা irqchip_aplic_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো aplic_তথ্য *pd;

	pd = sbi_zalloc(আকার(*pd));
	যদি (!pd)
		ফেরত SBI_ENOMEM;

	rc = fdt_parse_aplic_গ্রন্থি(fdt, nodeoff, pd);
	যদি (rc)
		যাও fail_free_তথ্য;

	যদি (pd->অঙ্ক_idc) {
		pd->idc_map = sbi_zalloc(আকার(*pd->idc_map) * pd->অঙ্ক_idc);
		যদি (!pd->idc_map) {
			rc = SBI_ENOMEM;
			যাও fail_free_তথ্য;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		যদি (rc)
			যাও fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	যদি (rc)
		যাও fail_free_idc_map;

	ফেরত 0;

fail_free_idc_map:
	যদি (pd->অঙ্ক_idc)
		sbi_free(pd->idc_map);
fail_free_তথ্য:
	sbi_free(pd);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
