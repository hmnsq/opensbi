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

स्थिर पूर्णाङ्क irqchip_imsic_update_hartid_सारणी(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
					     संरचना imsic_दत्त *id)
{
	ध्रुव fdt32_t *val;
	u32 phandle, hwirq, hartid;
	पूर्णाङ्क i, err, गणनम्, cpu_विस्थापनम्, cpu_intc_विस्थापनम्;

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
			प्रत्यागम SBI_EINVAL;

		विकल्प (hwirq) {
		पक्ष IRQ_M_EXT:
			err = imsic_map_hartid_गन्तव्यम्_दत्त(hartid, id, i / 2);
			यदि (err)
				प्रत्यागम err;
			भङ्ग;
		स्वभाव:
			भङ्ग;
		}
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क irqchip_imsic_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				   ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	पूर्णाङ्क rc;
	संरचना imsic_दत्त *id;

	id = sbi_zalloc(परिमाण(*id));
	यदि (!id)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_imsic_ग्रन्थिः(fdt, nodeoff, id);
	यदि (rc || !id->targets_mmode)
		गच्छ fail_free_दत्त;

	rc = imsic_cold_irqchip_init(id);
	यदि (rc)
		गच्छ fail_free_दत्त;

	rc = irqchip_imsic_update_hartid_सारणी(fdt, nodeoff, id);
	यदि (rc)
		गच्छ fail_free_दत्त;

	प्रत्यागम 0;

fail_free_दत्त:
	sbi_free(id);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_मेलनम् irqchip_imsic_मेलनम्[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_irqchip_imsic = {
	.मेलनम्_सारणी = irqchip_imsic_मेलनम्,
	.init = irqchip_imsic_cold_init,
};
