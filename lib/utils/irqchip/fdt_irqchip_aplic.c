/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/fdt_irqchip.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/aplic.h>

স্থির পূর্ণাঙ্ক irqchip_aplic_update_idc_map(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
					সংরচনা aplic_data *pd)
{
	পূর্ণাঙ্ক i, err, গুনতি, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ধ্রুৱ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		ঘূৰাই SBI_EINVAL;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	বাবে (i = 0; i < গুনতি; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুৱর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুৱর্ত;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			অনুৱর্ত;

		hartindex = sbi_hartid_to_hartindex(hartid);
		যদি (hartindex == -1U)
			অনুৱর্ত;

		pd->idc_map[i / 2] = hartindex;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক irqchip_aplic_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				   ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা aplic_data *pd;

	pd = sbi_zalloc(পরিমাণ(*pd));
	যদি (!pd)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_parse_aplic_গ্ৰন্থি(fdt, nodeoff, pd);
	যদি (rc)
		গচ্ছ fail_free_data;

	যদি (pd->অঙ্ক_idc) {
		pd->idc_map = sbi_zalloc(পরিমাণ(*pd->idc_map) * pd->অঙ্ক_idc);
		যদি (!pd->idc_map) {
			rc = SBI_ENOMEM;
			গচ্ছ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		যদি (rc)
			গচ্ছ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	যদি (rc)
		গচ্ছ fail_free_idc_map;

	ঘূৰাই 0;

fail_free_idc_map:
	যদি (pd->অঙ্ক_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	ঘূৰাই rc;
}

স্থির ধ্রুৱ সংরচনা fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
