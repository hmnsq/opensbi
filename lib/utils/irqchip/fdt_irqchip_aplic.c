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
#അന്തര്ഭാവ <sbi_utils/irqchip/aplic.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_aplic_update_idc_map(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
					ഘടന aplic_data *pd)
{
	പൂർണ്ണസംഖ്യ i, err, എണ്ണം, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	സ്ഥിരം fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &എണ്ണം);
	എങ്കിൽ (!val || എണ്ണം < വലുപ്പം(fdt32_t))
		തിരികെ SBI_EINVAL;
	എണ്ണം = എണ്ണം / വലുപ്പം(fdt32_t);

	വേണ്ടി (i = 0; i < എണ്ണം; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
		എങ്കിൽ (cpu_intc_offset < 0)
			തുടരുക;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		എങ്കിൽ (cpu_offset < 0)
			തുടരുക;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (err)
			തുടരുക;

		hartindex = sbi_hartid_to_hartindex(hartid);
		എങ്കിൽ (hartindex == -1U)
			തുടരുക;

		pd->idc_map[i / 2] = hartindex;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_aplic_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				   സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന aplic_data *pd;

	pd = sbi_zalloc(വലുപ്പം(*pd));
	എങ്കിൽ (!pd)
		തിരികെ SBI_ENOMEM;

	rc = fdt_parse_aplic_കണ്ണി(fdt, nodeoff, pd);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	എങ്കിൽ (pd->അക്കം_idc) {
		pd->idc_map = sbi_zalloc(വലുപ്പം(*pd->idc_map) * pd->അക്കം_idc);
		എങ്കിൽ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			പോകുക fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		എങ്കിൽ (rc)
			പോകുക fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	എങ്കിൽ (rc)
		പോകുക fail_free_idc_map;

	തിരികെ 0;

fail_free_idc_map:
	എങ്കിൽ (pd->അക്കം_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
