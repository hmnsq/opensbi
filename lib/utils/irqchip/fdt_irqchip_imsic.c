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
#समावेश <sbi_utils/irqchip/imsic.h>

अटल पूर्णांक irqchip_imsic_update_hartid_table(स्थिर व्योम *fdt, पूर्णांक nodeoff,
					     काष्ठा imsic_तथ्य *id)
{
	स्थिर fdt32_t *val;
	u32 phandle, hwirq, hartid;
	पूर्णांक i, err, गिनती, cpu_offset, cpu_intc_offset;

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
			वापस SBI_EINVAL;

		चयन (hwirq) {
		हाल IRQ_M_EXT:
			err = imsic_map_hartid_to_तथ्य(hartid, id, i / 2);
			अगर (err)
				वापस err;
			अवरोध;
		शेष:
			अवरोध;
		}
	}

	वापस 0;
}

अटल पूर्णांक irqchip_imsic_cold_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				   स्थिर काष्ठा fdt_match *match)
{
	पूर्णांक rc;
	काष्ठा imsic_तथ्य *id;

	id = sbi_zalloc(माप(*id));
	अगर (!id)
		वापस SBI_ENOMEM;

	rc = fdt_parse_imsic_गाँठ(fdt, nodeoff, id);
	अगर (rc || !id->targets_mmode)
		जाओ fail_free_तथ्य;

	rc = imsic_cold_irqchip_init(id);
	अगर (rc)
		जाओ fail_free_तथ्य;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	अगर (rc)
		जाओ fail_free_तथ्य;

	वापस 0;

fail_free_तथ्य:
	sbi_free(id);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
