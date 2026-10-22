/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/irqchip/fdt_irqchip.h>
#అన్తర్భావ <sbi_utils/irqchip/aplic.h>

స్థిర పూర్ణాంకం irqchip_aplic_update_idc_map(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
					నిర్మాణం aplic_దత్తాంశం *pd)
{
	పూర్ణాంకం i, err, లెక్క, cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;
	u32 phandle, hartid, hartindex;
	స్థిరరాశి fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &లెక్క);
	ఒకవేళ (!val || లెక్క < పరిమాణం(fdt32_t))
		తిరిగి SBI_EINVAL;
	లెక్క = లెక్క / పరిమాణం(fdt32_t);

	కొరకు (i = 0; i < లెక్క; i += 2) {
		phandle = fdt32_గమ్యం_cpu(val[i]);

		cpu_intc_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);
		ఒకవేళ (cpu_intc_స్థానభ్రంశం < 0)
			కొనసాగు;

		cpu_స్థానభ్రంశం = fdt_మాతృ_స్థానభ్రంశం(fdt, cpu_intc_స్థానభ్రంశం);
		ఒకవేళ (cpu_స్థానభ్రంశం < 0)
			కొనసాగు;

		err = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);
		ఒకవేళ (err)
			కొనసాగు;

		hartindex = sbi_hartid_గమ్యం_hartindex(hartid);
		ఒకవేళ (hartindex == -1U)
			కొనసాగు;

		pd->idc_map[i / 2] = hartindex;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం irqchip_aplic_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం aplic_దత్తాంశం *pd;

	pd = sbi_zalloc(పరిమాణం(*pd));
	ఒకవేళ (!pd)
		తిరిగి SBI_ENOMEM;

	rc = fdt_parse_aplic_కణుపు(fdt, nodeoff, pd);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	ఒకవేళ (pd->అంకె_idc) {
		pd->idc_map = sbi_zalloc(పరిమాణం(*pd->idc_map) * pd->అంకె_idc);
		ఒకవేళ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			వెళ్ళు fail_free_దత్తాంశం;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ఒకవేళ (rc)
			వెళ్ళు fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_idc_map;

	తిరిగి 0;

fail_free_idc_map:
	ఒకవేళ (pd->అంకె_idc)
		sbi_free(pd->idc_map);
fail_free_దత్తాంశం:
	sbi_free(pd);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక irqchip_aplic_సరిపోలిక[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_irqchip_aplic = {
	.సరిపోలిక_పట్టిక = irqchip_aplic_సరిపోలిక,
	.init = irqchip_aplic_cold_init,
};
