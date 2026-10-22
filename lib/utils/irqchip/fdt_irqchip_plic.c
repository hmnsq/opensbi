/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/irqchip/fdt_irqchip.h>
#அந்தர்பாவ <sbi_utils/irqchip/plic.h>

நிலையான முழு irqchip_plic_update_context_map(மாறிலி வெறுமை *fdt, முழு nodeoff,
					   கட்டமைப்பு plic_data *pd)
{
	மாறிலி fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	முழு i, err, மொத்தம், cpu_offset, cpu_intc_offset;

	க்கு (i = 0; i < sbi_hart_மொத்தம்(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &மொத்தம்);
	எனில் (!val || மொத்தம் < அளவு(fdt32_t))
		திரும்பு SBI_EINVAL;
	மொத்தம் = மொத்தம் / அளவு(fdt32_t);

	க்கு (i = 0; i < மொத்தம்; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_முனை_offset_by_phandle(fdt, phandle);
		எனில் (cpu_intc_offset < 0)
			தொடர்;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		எனில் (cpu_offset < 0)
			தொடர்;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		எனில் (err)
			தொடர்;

		hartindex = sbi_hartid_to_hartindex(hartid);
		எனில் (hartindex == -1U)
			தொடர்;

		தேர்வு (hwirq) {
		நிலை IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			நிறுத்து;
		நிலை IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			நிறுத்து;
		}
	}

	திரும்பு 0;
}

நிலையான முழு irqchip_plic_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				  மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_மொத்தம்()));
	எனில் (!pd)
		திரும்பு SBI_ENOMEM;

	rc = fdt_parse_plic_முனை(fdt, nodeoff, pd);
	எனில் (rc)
		செல் fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	எனில் (rc)
		செல் fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	எனில் (rc)
		செல் fail_free_data;

	திரும்பு 0;

fail_free_data:
	sbi_free(pd);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (வெறுமை *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

மாறிலி கட்டமைப்பு fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
