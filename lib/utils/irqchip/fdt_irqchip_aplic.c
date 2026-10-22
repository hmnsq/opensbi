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
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/aplic.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_aplic_update_idc_map(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
					ସଂରଚନା aplic_data *pd)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, err, ଗଣନା_2, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ଅଚଳ fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ଗଣନା_2);
	ଯଦି (!val || ଗଣନା_2 < ଆକାର(fdt32_t))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଗଣନା_2 = ଗଣନା_2 / ଆକାର(fdt32_t);

	ପାଇଁ (i = 0; i < ଗଣନା_2; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_ଗଣ୍ଠି_offset_by_phandle(fdt, phandle);
		ଯଦି (cpu_intc_offset < 0)
			ଜାରି;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ଯଦି (cpu_offset < 0)
			ଜାରି;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ଯଦି (err)
			ଜାରି;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ଯଦି (hartindex == -1U)
			ଜାରି;

		pd->idc_map[i / 2] = hartindex;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_aplic_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				   ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା aplic_data *pd;

	pd = sbi_zalloc(ଆକାର(*pd));
	ଯଦି (!pd)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_parse_aplic_ଗଣ୍ଠି(fdt, nodeoff, pd);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	ଯଦି (pd->ଅଙ୍କ_idc) {
		pd->idc_map = sbi_zalloc(ଆକାର(*pd->idc_map) * pd->ଅଙ୍କ_idc);
		ଯଦି (!pd->idc_map) {
			rc = SBI_ENOMEM;
			ଯାଅ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ଯଦି (rc)
			ଯାଅ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ଯଦି (rc)
		ଯାଅ fail_free_idc_map;

	ଫେରନ୍ତୁ 0;

fail_free_idc_map:
	ଯଦି (pd->ଅଙ୍କ_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
