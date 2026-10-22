/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/fdt_irqchip.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/imsic.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_imsic_update_hartid_table(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
					     ഘടന imsic_data *id)
{
	സ്ഥിരം fdt32_t *val;
	u32 phandle, hwirq, hartid;
	പൂർണ്ണസംഖ്യ i, err, എണ്ണം, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &എണ്ണം);
	എങ്കിൽ (!val || എണ്ണം < വലുപ്പം(fdt32_t))
		തിരികെ SBI_EINVAL;
	എണ്ണം = എണ്ണം / വലുപ്പം(fdt32_t);

	വേണ്ടി (i = 0; i < എണ്ണം; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
		എങ്കിൽ (cpu_intc_offset < 0)
			തുടരുക;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		എങ്കിൽ (cpu_offset < 0)
			തുടരുക;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (err)
			തിരികെ SBI_EINVAL;

		തിരഞ്ഞെടുപ്പ് (hwirq) {
		അവസ്ഥ IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			എങ്കിൽ (err)
				തിരികെ err;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_imsic_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന imsic_data *id;

	id = sbi_zalloc(വലുപ്പം(*id));
	എങ്കിൽ (!id)
		തിരികെ SBI_ENOMEM;

	rc = fdt_parse_imsic_കണ്ണി(fdt, nodeoff, id);
	എങ്കിൽ (rc || !id->targets_mmode)
		പോകുക fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	തിരികെ 0;

fail_free_data:
	sbi_free(id);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
