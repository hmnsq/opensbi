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

स्थिर पूर्णाङ्क irqchip_imsic_update_hartid_table(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
					     संरचना imsic_data *id)
{
	अचर fdt32_t *val;
	u32 phandle, hwirq, hartid;
	पूर्णाङ्क i, err, गन्ती, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गन्ती);
	यदि (!val || गन्ती < परिमाण(fdt32_t))
		फर्काऊ SBI_EINVAL;
	गन्ती = गन्ती / परिमाण(fdt32_t);

	लागि (i = 0; i < गन्ती; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_गाँठो_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			जारी;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (err)
			फर्काऊ SBI_EINVAL;

		विकल्प (hwirq) {
		पक्ष IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			यदि (err)
				फर्काऊ err;
			रोक;
		स्वभाव:
			रोक;
		}
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क irqchip_imsic_cold_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				   अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना imsic_data *id;

	id = sbi_zalloc(परिमाण(*id));
	यदि (!id)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_parse_imsic_गाँठो(fdt, nodeoff, id);
	यदि (rc || !id->targets_mmode)
		गच्छ fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	यदि (rc)
		गच्छ fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	यदि (rc)
		गच्छ fail_free_data;

	फर्काऊ 0;

fail_free_data:
	sbi_free(id);
	फर्काऊ rc;
}

स्थिर अचर संरचना fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

अचर संरचना fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
