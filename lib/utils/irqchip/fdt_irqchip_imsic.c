/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/fdt_irqchip.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/imsic.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_imsic_update_hartid_table(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
					     ସଂରଚନା imsic_data *id)
{
	ଅଚଳ fdt32_t *val;
	u32 phandle, hwirq, hartid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, err, ଗଣନା_2, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ଗଣନା_2);
	ଯଦି (!val || ଗଣନା_2 < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଗଣନା_2 = ଗଣନା_2 / ଆକାର(fdt32_t);

	ପାଇଁ (i = 0; i < ଗଣନା_2; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
		ଯଦି (cpu_intc_offset < 0)
			ଜାରି;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ଯଦି (cpu_offset < 0)
			ଜାରି;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (err)
			ଫେରନ୍ତୁ SBI_EINVAL;

		ବାଛ (hwirq) {
		ସ୍ଥିତି IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			ଯଦି (err)
				ଫେରନ୍ତୁ err;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_imsic_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା imsic_data *id;

	id = sbi_zalloc(ଆକାର(*id));
	ଯଦି (!id)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_parse_imsic_ଗଣ୍ଠି(fdt, nodeoff, id);
	ଯଦି (rc || !id->targets_mmode)
		ଯାଅ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	ଫେରନ୍ତୁ 0;

fail_free_data:
	sbi_free(id);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
