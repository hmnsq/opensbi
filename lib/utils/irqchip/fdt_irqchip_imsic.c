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
#అన్తర్భావ <sbi_utils/irqchip/imsic.h>

స్థిర పూర్ణాంకం irqchip_imsic_update_hartid_పట్టిక(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
					     నిర్మాణం imsic_దత్తాంశం *id)
{
	స్థిరరాశి fdt32_t *val;
	u32 phandle, hwirq, hartid;
	పూర్ణాంకం i, err, లెక్క, cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;

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
			తిరిగి SBI_EINVAL;

		ఎంపిక (hwirq) {
		స్థితి IRQ_M_EXT:
			err = imsic_map_hartid_గమ్యం_దత్తాంశం(hartid, id, i / 2);
			ఒకవేళ (err)
				తిరిగి err;
			ఆపు;
		అప్రమేయ:
			ఆపు;
		}
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం irqchip_imsic_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				   స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	పూర్ణాంకం rc;
	నిర్మాణం imsic_దత్తాంశం *id;

	id = sbi_zalloc(పరిమాణం(*id));
	ఒకవేళ (!id)
		తిరిగి SBI_ENOMEM;

	rc = fdt_parse_imsic_కణుపు(fdt, nodeoff, id);
	ఒకవేళ (rc || !id->targets_mmode)
		వెళ్ళు fail_free_దత్తాంశం;

	rc = imsic_cold_irqchip_init(id);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	rc = irqchip_imsic_update_hartid_పట్టిక(fdt, nodeoff, id);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_దత్తాంశం;

	తిరిగి 0;

fail_free_దత్తాంశం:
	sbi_free(id);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక irqchip_imsic_సరిపోలిక[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_irqchip_imsic = {
	.సరిపోలిక_పట్టిక = irqchip_imsic_సరిపోలిక,
	.init = irqchip_imsic_cold_init,
};
