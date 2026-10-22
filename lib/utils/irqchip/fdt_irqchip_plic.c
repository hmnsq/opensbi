/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/irqchip/fdt_irqchip.h>
#अन्तर्भाव <sbi_utils/irqchip/plic.h>

स्थिर पूर्णाङ्क irqchip_plic_update_context_map(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
					   संरचना plic_दत्त *pd)
{
	ध्रुव fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	पूर्णाङ्क i, err, गणनम्, cpu_विस्थापनम्, cpu_intc_विस्थापनम्;

	प्रति (i = 0; i < sbi_hart_गणनम्(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गणनम्);
	यदि (!val || गणनम् < परिमाण(fdt32_t))
		प्रत्यागम SBI_EINVAL;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	प्रति (i = 0; i < गणनम्; i += 2) {
		phandle = fdt32_गन्तव्यम्_cpu(val[i]);
		hwirq = fdt32_गन्तव्यम्_cpu(val[i + 1]);

		cpu_intc_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, phandle);
		यदि (cpu_intc_विस्थापनम् < 0)
			अनुवर्त;

		cpu_विस्थापनम् = fdt_parent_विस्थापनम्(fdt, cpu_intc_विस्थापनम्);
		यदि (cpu_विस्थापनम् < 0)
			अनुवर्त;

		err = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);
		यदि (err)
			अनुवर्त;

		hartindex = sbi_hartid_गन्तव्यम्_hartindex(hartid);
		यदि (hartindex == -1U)
			अनुवर्त;

		विकल्प (hwirq) {
		पक्ष IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			भङ्ग;
		पक्ष IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			भङ्ग;
		}
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क irqchip_plic_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				  ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना plic_दत्त *pd;

	pd = sbi_zalloc(PLIC_दत्त_आकार(sbi_hart_गणनम्()));
	यदि (!pd)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_plic_ग्रन्थिः(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_दत्त;

	pd->flags = (अचिह्नित दीर्घ)मेलनम्->दत्त;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_दत्त;

	rc = plic_cold_irqchip_init(pd);
	यदि (rc)
		गच्छ fail_free_दत्त;

	प्रत्यागम 0;

fail_free_दत्त:
	sbi_free(pd);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_मेलनम् irqchip_plic_मेलनम्[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .दत्त = (शून्य *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ध्रुव संरचना fdt_driver fdt_irqchip_plic = {
	.मेलनम्_सारणी = irqchip_plic_मेलनम्,
	.init = irqchip_plic_cold_init,
};
