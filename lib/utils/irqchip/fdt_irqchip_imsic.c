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

स्थिर पूर्णाङ्क irqchip_imsic_update_hartid_table(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
					     संरचना imsic_data *id)
{
	ध्रुव fdt32_t *val;
	u32 phandle, hwirq, hartid;
	पूर्णाङ्क i, err, गणनम्, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गणनम्);
	सचे (!val || गणनम् < परिमाण(fdt32_t))
		पटिनिवत्त SBI_EINVAL;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	अत्थाय (i = 0; i < गणनम्; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);
		सचे (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		सचे (cpu_offset < 0)
			अनुवर्त;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (err)
			पटिनिवत्त SBI_EINVAL;

		विकल्प (hwirq) {
		पक्ष IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			सचे (err)
				पटिनिवत्त err;
			भङ्ग;
		स्वभाव:
			भङ्ग;
		}
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क irqchip_imsic_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				   ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना imsic_data *id;

	id = sbi_zalloc(परिमाण(*id));
	सचे (!id)
		पटिनिवत्त SBI_ENOMEM;

	rc = fdt_parse_imsic_ग्रन्थिः(fdt, nodeoff, id);
	सचे (rc || !id->targets_mmode)
		गच्छ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	सचे (rc)
		गच्छ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	सचे (rc)
		गच्छ fail_free_data;

	पटिनिवत्त 0;

fail_free_data:
	sbi_free(id);
	पटिनिवत्त rc;
}

स्थिर ध्रुव संरचना fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
