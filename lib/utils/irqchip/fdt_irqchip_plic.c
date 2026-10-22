/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/irqchip/fdt_irqchip.h>
#समावेश <sbi_utils/irqchip/plic.h>

अटल पूर्णांक irqchip_plic_update_context_map(स्थिर व्योम *fdt, पूर्णांक nodeoff,
					   काष्ठा plic_तथ्य *pd)
{
	स्थिर fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	पूर्णांक i, err, गिनती, cpu_offset, cpu_intc_offset;

	क्रम (i = 0; i < sbi_hart_गिनती(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गिनती);
	अगर (!val || गिनती < माप(fdt32_t))
		वापस SBI_EINVAL;
	गिनती = गिनती / माप(fdt32_t);

	क्रम (i = 0; i < गिनती; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_गाँठ_offset_by_phandle(fdt, phandle);
		अगर (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		अगर (cpu_offset < 0)
			जारी;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		अगर (err)
			जारी;

		hartindex = sbi_hartid_to_hartindex(hartid);
		अगर (hartindex == -1U)
			जारी;

		चयन (hwirq) {
		हाल IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			अवरोध;
		हाल IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			अवरोध;
		}
	}

	वापस 0;
}

अटल पूर्णांक irqchip_plic_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				  स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा plic_तथ्य *pd;

	pd = sbi_zalloc(PLIC_तथ्य_आकार(sbi_hart_गिनती()));
	अगर (!pd)
		वापस SBI_ENOMEM;

	rc = fdt_parse_plic_गाँठ(fdt, nodeoff, pd);
	अगर (rc)
		जाओ fail_free_तथ्य;

	pd->flags = (अचिन्हित दीर्घ)match->तथ्य;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	अगर (rc)
		जाओ fail_free_तथ्य;

	rc = plic_cold_irqchip_init(pd);
	अगर (rc)
		जाओ fail_free_तथ्य;

	वापस 0;

fail_free_तथ्य:
	sbi_free(pd);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .तथ्य = (व्योम *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

स्थिर काष्ठा fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
