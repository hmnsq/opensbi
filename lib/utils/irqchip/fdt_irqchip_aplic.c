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
					संरचना aplic_data *pd)
{
	पूर्णाङ्क i, err, गणनम्, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ध्रुव fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गणनम्);
	सचे (!val || गणनम् < परिमाण(fdt32_t))
		पटिनिवत्त SBI_EINVAL;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	अत्थाय (i = 0; i < गणनम्; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);
		सचे (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		सचे (cpu_offset < 0)
			अनुवर्त;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (err)
			अनुवर्त;

		hartindex = sbi_hartid_to_hartindex(hartid);
		सचे (hartindex == -1U)
			अनुवर्त;

		pd->idc_map[i / 2] = hartindex;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क irqchip_aplic_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				   ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना aplic_data *pd;

	pd = sbi_zalloc(परिमाण(*pd));
	सचे (!pd)
		पटिनिवत्त SBI_ENOMEM;

	rc = fdt_parse_aplic_ग्रन्थिः(fdt, nodeoff, pd);
	सचे (rc)
		गच्छ fail_free_data;

	सचे (pd->अङ्कः_idc) {
		pd->idc_map = sbi_zalloc(परिमाण(*pd->idc_map) * pd->अङ्कः_idc);
		सचे (!pd->idc_map) {
			rc = SBI_ENOMEM;
			गच्छ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		सचे (rc)
			गच्छ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	सचे (rc)
		गच्छ fail_free_idc_map;

	पटिनिवत्त 0;

fail_free_idc_map:
	सचे (pd->अङ्कः_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	पटिनिवत्त rc;
}

स्थिर ध्रुव संरचना fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
