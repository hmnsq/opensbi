/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/irqchip/fdt_irqchip.h>
#समावेश <sbi_utils/irqchip/aplic.h>

अटल पूर्णांक irqchip_aplic_update_idc_map(स्थिर व्योम *fdt, पूर्णांक nodeoff,
					काष्ठा aplic_तथ्य *pd)
{
	पूर्णांक i, err, गिनती, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	स्थिर fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &गिनती);
	अगर (!val || गिनती < माप(fdt32_t))
		वापस SBI_EINVAL;
	गिनती = गिनती / माप(fdt32_t);

	क्रम (i = 0; i < गिनती; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	वापस 0;
}

अटल पूर्णांक irqchip_aplic_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा aplic_तथ्य *pd;

	pd = sbi_zalloc(माप(*pd));
	अगर (!pd)
		वापस SBI_ENOMEM;

	rc = fdt_parse_aplic_गाँठ(fdt, nodeoff, pd);
	अगर (rc)
		जाओ fail_free_तथ्य;

	अगर (pd->अंक_idc) {
		pd->idc_map = sbi_zalloc(माप(*pd->idc_map) * pd->अंक_idc);
		अगर (!pd->idc_map) {
			rc = SBI_ENOMEM;
			जाओ fail_free_तथ्य;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		अगर (rc)
			जाओ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	अगर (rc)
		जाओ fail_free_idc_map;

	वापस 0;

fail_free_idc_map:
	अगर (pd->अंक_idc)
		sbi_free(pd->idc_map);
fail_free_तथ्य:
	sbi_free(pd);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
