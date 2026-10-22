/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/irqchip/fdt_irqchip.h>
#అన్తర్భావ <sbi_utils/irqchip/plic.h>

స్థిర పూర్ణాంకం irqchip_plic_update_context_map(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
					   నిర్మాణం plic_దత్తాంశం *pd)
{
	స్థిరరాశి fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	పూర్ణాంకం i, err, లెక్క, cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;

	కొరకు (i = 0; i < sbi_hart_లెక్క(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &లెక్క);
	ఒకవేళ (!val || లెక్క < పరిమాణం(fdt32_t))
		తిరిగి SBI_EINVAL;
	లెక్క = లెక్క / పరిమాణం(fdt32_t);

	కొరకు (i = 0; i < లెక్క; i += 2) {
		phandle = fdt32_గమ్యం_cpu(val[i]);
		hwirq = fdt32_గమ్యం_cpu(val[i + 1]);

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

		ఎంపిక (hwirq) {
		స్థితి IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			ఆపు;
		స్థితి IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			ఆపు;
		}
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం irqchip_plic_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				  స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం plic_దత్తాంశం *pd;

	pd = sbi_zalloc(PLIC_దత్తాంశం_కొలత(sbi_hart_లెక్క()));
	ఒకవేళ (!pd)
		తిరిగి SBI_ENOMEM;

	rc = fdt_parse_plic_కణుపు(fdt, nodeoff, pd);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	pd->flags = (గుర్తులేని దీర్ఘ)సరిపోలిక->దత్తాంశం;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	rc = plic_cold_irqchip_init(pd);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	తిరిగి 0;

fail_free_దత్తాంశం:
	sbi_free(pd);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక irqchip_plic_సరిపోలిక[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .దత్తాంశం = (శూన్య *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_irqchip_plic = {
	.సరిపోలిక_పట్టిక = irqchip_plic_సరిపోలిక,
	.init = irqchip_plic_cold_init,
};
