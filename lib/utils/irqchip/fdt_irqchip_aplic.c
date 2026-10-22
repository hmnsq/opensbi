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

स्थिर पूर्णाङ्क irqchip_aplic_update_idc_map(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
					संरचना aplic_data *pd)
{
	पूर्णाङ्क i, err, गन्ती, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	अचर fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गन्ती);
	यदि (!val || गन्ती < परिमाण(fdt32_t))
		फर्काऊ SBI_EINVAL;
	गन्ती = गन्ती / परिमाण(fdt32_t);

	लागि (i = 0; i < गन्ती; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_गाँठो_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			जारी;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (err)
			जारी;

		hartindex = sbi_hartid_to_hartindex(hartid);
		यदि (hartindex == -1U)
			जारी;

		pd->idc_map[i / 2] = hartindex;
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क irqchip_aplic_cold_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				   अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना aplic_data *pd;

	pd = sbi_zalloc(परिमाण(*pd));
	यदि (!pd)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_parse_aplic_गाँठो(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_data;

	यदि (pd->अङ्क_idc) {
		pd->idc_map = sbi_zalloc(परिमाण(*pd->idc_map) * pd->अङ्क_idc);
		यदि (!pd->idc_map) {
			rc = SBI_ENOMEM;
			गच्छ fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		यदि (rc)
			गच्छ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	यदि (rc)
		गच्छ fail_free_idc_map;

	फर्काऊ 0;

fail_free_idc_map:
	यदि (pd->अङ्क_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	फर्काऊ rc;
}

स्थिर अचर संरचना fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

अचर संरचना fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
