/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/fdt_irqchip.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/irqchip/plic.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_plic_update_context_map(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
					   ସଂରଚନା plic_data *pd)
{
	ଅଚଳ fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, err, ଗଣନା_2, cpu_offset, cpu_intc_offset;

	ପାଇଁ (i = 0; i < sbi_hart_ଗଣନା(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			ଜାରି;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ଯଦି (hartindex == -1U)
			ଜାରି;

		ବାଛ (hwirq) {
		ସ୍ଥିତି IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ଭାଙ୍ଗ;
		}
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା irqchip_plic_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				  ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_ଗଣନା()));
	ଯଦି (!pd)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_parse_plic_ଗଣ୍ଠି(fdt, nodeoff, pd);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	ଯଦି (rc)
		ଯାଅ fail_free_data;

	ଫେରନ୍ତୁ 0;

fail_free_data:
	sbi_free(pd);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (ଶୂନ୍ୟ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
