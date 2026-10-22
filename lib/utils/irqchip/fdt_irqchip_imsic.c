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
#অন্তর্ভাৱ <sbi_utils/irqchip/imsic.h>

স্থির পূর্ণাঙ্ক irqchip_imsic_update_hartid_table(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
					     সংরচনা imsic_data *id)
{
	ধ্রুৱ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	পূর্ণাঙ্ক i, err, গুনতি, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		ঘূৰাই SBI_EINVAL;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	বাবে (i = 0; i < গুনতি; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুৱর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুৱর্ত;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			ঘূৰাই SBI_EINVAL;

		ৱিকল্প (hwirq) {
		পক্ষ IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			যদি (err)
				ঘূৰাই err;
			ভঙ্গ;
		স্ৱভাৱ:
			ভঙ্গ;
		}
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক irqchip_imsic_cold_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				   ধ্রুৱ সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	সংরচনা imsic_data *id;

	id = sbi_zalloc(পরিমাণ(*id));
	যদি (!id)
		ঘূৰাই SBI_ENOMEM;

	rc = fdt_parse_imsic_গ্ৰন্থি(fdt, nodeoff, id);
	যদি (rc || !id->targets_mmode)
		গচ্ছ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	যদি (rc)
		গচ্ছ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	যদি (rc)
		গচ্ছ fail_free_data;

	ঘূৰাই 0;

fail_free_data:
	sbi_free(id);
	ঘূৰাই rc;
}

স্থির ধ্রুৱ সংরচনা fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
