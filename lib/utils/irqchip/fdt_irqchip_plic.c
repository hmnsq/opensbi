/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/fdt_irqchip.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/plic.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_plic_update_context_map(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
					   ഘടന plic_data *pd)
{
	സ്ഥിരം fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	പൂർണ്ണസംഖ്യ i, err, എണ്ണം, cpu_offset, cpu_intc_offset;

	വേണ്ടി (i = 0; i < sbi_hart_എണ്ണം(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			തുടരുക;

		hartindex = sbi_hartid_to_hartindex(hartid);
		എങ്കിൽ (hartindex == -1U)
			തുടരുക;

		തിരഞ്ഞെടുപ്പ് (hwirq) {
		അവസ്ഥ IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			നിർത്തുക;
		അവസ്ഥ IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			നിർത്തുക;
		}
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ irqchip_plic_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				  സ്ഥിരം ഘടന fdt_match *match)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_എണ്ണം()));
	എങ്കിൽ (!pd)
		തിരികെ SBI_ENOMEM;

	rc = fdt_parse_plic_കണ്ണി(fdt, nodeoff, pd);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	എങ്കിൽ (rc)
		പോകുക fail_free_data;

	തിരികെ 0;

fail_free_data:
	sbi_free(pd);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (ശൂന്യം *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

സ്ഥിരം ഘടന fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
