/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/irqchip/fdt_irqchip.h>
#अन्तर्भाव <sbi_utils/irqchip/aplic.h>

स्थिर पूर्णाङ्क irqchip_aplic_update_idc_map(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
					संरचना aplic_दत्त *pd)
{
	पूर्णाङ्क i, err, गणनम्, cpu_विस्थापनम्, cpu_intc_विस्थापनम्;
	u32 phandle, hartid, hartindex;
	ध्रुव fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गणनम्);
	यदि (!val || गणनम् < परिमाण(fdt32_t))
		प्रत्यागम SBI_EINVAL;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	प्रति (i = 0; i < गणनम्; i += 2) {
		phandle = fdt32_गन्तव्यम्_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क irqchip_aplic_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				   ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना aplic_दत्त *pd;

	pd = sbi_zalloc(परिमाण(*pd));
	यदि (!pd)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_aplic_ग्रन्थिः(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_दत्त;

	यदि (pd->अङ्कः_idc) {
		pd->idc_map = sbi_zalloc(परिमाण(*pd->idc_map) * pd->अङ्कः_idc);
		यदि (!pd->idc_map) {
			rc = SBI_ENOMEM;
			गच्छ fail_free_दत्त;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		यदि (rc)
			गच्छ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	यदि (rc)
		गच्छ fail_free_idc_map;

	प्रत्यागम 0;

fail_free_idc_map:
	यदि (pd->अङ्कः_idc)
		sbi_free(pd->idc_map);
fail_free_दत्त:
	sbi_free(pd);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_मेलनम् irqchip_aplic_मेलनम्[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_irqchip_aplic = {
	.मेलनम्_सारणी = irqchip_aplic_मेलनम्,
	.init = irqchip_aplic_cold_init,
};
