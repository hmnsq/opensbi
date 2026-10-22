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
#अन्तर्भाव <sbi_utils/irqchip/imsic.h>

स्थिर पूर्णांक irqchip_imsic_update_hartid_table(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
					     संरचना imsic_data *id)
{
	स्थिरांक fdt32_t *val;
	u32 phandle, hwirq, hartid;
	पूर्णांक i, err, मोजणी, cpu_offset, cpu_intc_offset;

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
			परत SBI_EINVAL;

		निवड (hwirq) {
		स्थिती IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			जर (err)
				परत err;
			थांबा;
		पूर्वनिर्धारित:
			थांबा;
		}
	}

	परत 0;
}

स्थिर पूर्णांक irqchip_imsic_cold_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				   स्थिरांक संरचना fdt_match *match)
{
	पूर्णांक rc;
	संरचना imsic_data *id;

	id = sbi_zalloc(आकार(*id));
	जर (!id)
		परत SBI_ENOMEM;

	rc = fdt_parse_imsic_गाठ(fdt, nodeoff, id);
	जर (rc || !id->targets_mmode)
		जा fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	जर (rc)
		जा fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	जर (rc)
		जा fail_free_data;

	परत 0;

fail_free_data:
	sbi_free(id);
	परत rc;
}

स्थिर स्थिरांक संरचना fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
