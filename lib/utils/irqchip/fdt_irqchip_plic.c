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

स्थिर पूर्णांक irqchip_plic_update_context_map(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
					   संरचना plic_data *pd)
{
	स्थिरांक fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	पूर्णांक i, err, मोजणी, cpu_offset, cpu_intc_offset;

	साठी (i = 0; i < sbi_hart_मोजणी(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &मोजणी);
	जर (!val || मोजणी < आकार(fdt32_t))
		परत SBI_EINVAL;
	मोजणी = मोजणी / आकार(fdt32_t);

	साठी (i = 0; i < मोजणी; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

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

		निवड (hwirq) {
		स्थिती IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			थांबा;
		स्थिती IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			थांबा;
		}
	}

	परत 0;
}

स्थिर पूर्णांक irqchip_plic_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				  स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_मोजणी()));
	जर (!pd)
		परत SBI_ENOMEM;

	rc = fdt_parse_plic_गाठ(fdt, nodeoff, pd);
	जर (rc)
		जा fail_free_data;

	pd->flags = (अचिन्हित दीर्घ)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	जर (rc)
		जा fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	जर (rc)
		जा fail_free_data;

	परत 0;

fail_free_data:
	sbi_free(pd);
	परत rc;
}

स्थिर स्थिरांक संरचना fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (शून्य *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

स्थिरांक संरचना fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
