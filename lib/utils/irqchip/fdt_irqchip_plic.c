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

स्थिर पूर्णाङ्क irqchip_plic_update_context_map(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
					   संरचना plic_data *pd)
{
	अचर fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	पूर्णाङ्क i, err, गन्ती, cpu_offset, cpu_intc_offset;

	लागि (i = 0; i < sbi_hart_गन्ती(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			जारी;

		hartindex = sbi_hartid_to_hartindex(hartid);
		यदि (hartindex == -1U)
			जारी;

		विकल्प (hwirq) {
		पक्ष IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			रोक;
		पक्ष IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			रोक;
		}
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क irqchip_plic_cold_init(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				  अचर संरचना fdt_match *match)
{
	पूर्णाङ्क rc;
	संरचना plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_गन्ती()));
	यदि (!pd)
		फर्काऊ SBI_ENOMEM;

	rc = fdt_parse_plic_गाँठो(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_data;

	pd->flags = (अचिन्हित दीर्घ)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	यदि (rc)
		गच्छ fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	यदि (rc)
		गच्छ fail_free_data;

	फर्काऊ 0;

fail_free_data:
	sbi_free(pd);
	फर्काऊ rc;
}

स्थिर अचर संरचना fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (शून्य *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

अचर संरचना fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
