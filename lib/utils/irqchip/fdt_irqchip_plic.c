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
					   संरचना plic_data *pd)
{
	ध्रुव fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	पूर्णाङ्क i, err, गिनती, cpu_offset, cpu_intc_offset;

	प्रति (i = 0; i < sbi_hart_गिनती(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गिनती);
	यदि (!val || गिनती < परिमाण(fdt32_t))
		प्रत्यागम SBI_EINVAL;
	गिनती = गिनती / परिमाण(fdt32_t);

	प्रति (i = 0; i < गिनती; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_गाँठ_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			अनुवर्त;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (err)
			अनुवर्त;

		hartindex = sbi_hartid_to_hartindex(hartid);
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
				  ध्रुव संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_गिनती()));
	यदि (!pd)
		प्रत्यागम SBI_ENOMEM;

	rc = fdt_parse_plic_गाँठ(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	यदि (rc)
		गच्छ fail_free_data;

	प्रत्यागम 0;

fail_free_data:
	sbi_free(pd);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (शून्य *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ध्रुव संरचना fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
