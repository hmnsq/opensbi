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
#অন্তর্ভাব <sbi_utils/irqchip/imsic.h>

স্থির পূর্ণসংখ্যা irqchip_imsic_update_hartid_table(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
					     কাঠামো imsic_তথ্য *id)
{
	ধ্রুবক fdt32_t *val;
	u32 phandle, hwirq, hartid;
	পূর্ণসংখ্যা i, err, গুনতি, cpu_offset, cpu_intc_offset;

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
			ফেরত SBI_EINVAL;

		নির্বাচন (hwirq) {
		ক্ষেত্র IRQ_M_EXT:
			err = imsic_map_hartid_to_তথ্য(hartid, id, i / 2);
			যদি (err)
				ফেরত err;
			বিরতি;
		পূর্বনির্ধারিত:
			বিরতি;
		}
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা irqchip_imsic_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				   ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	কাঠামো imsic_তথ্য *id;

	id = sbi_zalloc(আকার(*id));
	যদি (!id)
		ফেরত SBI_ENOMEM;

	rc = fdt_parse_imsic_গ্রন্থি(fdt, nodeoff, id);
	যদি (rc || !id->targets_mmode)
		যাও fail_free_তথ্য;

	rc = imsic_cold_irqchip_init(id);
	যদি (rc)
		যাও fail_free_তথ্য;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	যদি (rc)
		যাও fail_free_তথ্য;

	ফেরত 0;

fail_free_তথ্য:
	sbi_free(id);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
