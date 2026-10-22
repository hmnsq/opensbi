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

स्थिर पूर्णांक irqchip_aplic_update_idc_map(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
					संरचना aplic_data *pd)
{
	पूर्णांक i, err, मोजणी, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	स्थिरांक fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &मोजणी);
	जर (!val || मोजणी < आकार(fdt32_t))
		परत SBI_EINVAL;
	मोजणी = मोजणी / आकार(fdt32_t);

	साठी (i = 0; i < मोजणी; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_गाठ_offset_by_phandle(fdt, phandle);
		जर (cpu_intc_offset < 0)
			सुरूठेवा;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		जर (cpu_offset < 0)
			सुरूठेवा;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जर (err)
			सुरूठेवा;

		hartindex = sbi_hartid_to_hartindex(hartid);
		जर (hartindex == -1U)
			सुरूठेवा;

		pd->idc_map[i / 2] = hartindex;
	}

	परत 0;
}

स्थिर पूर्णांक irqchip_aplic_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				   स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना aplic_data *pd;

	pd = sbi_zalloc(आकार(*pd));
	जर (!pd)
		परत SBI_ENOMEM;

	rc = fdt_parse_aplic_गाठ(fdt, nodeoff, pd);
	जर (rc)
		जा fail_free_data;

	जर (pd->अंक_idc) {
		pd->idc_map = sbi_zalloc(आकार(*pd->idc_map) * pd->अंक_idc);
		जर (!pd->idc_map) {
			rc = SBI_ENOMEM;
			जा fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		जर (rc)
			जा fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	जर (rc)
		जा fail_free_idc_map;

	परत 0;

fail_free_idc_map:
	जर (pd->अंक_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	परत rc;
}

स्थिर स्थिरांक संरचना fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
