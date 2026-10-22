/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/irqchip/fdt_irqchip.h>
#அந்தர்பாவ <sbi_utils/irqchip/aplic.h>

நிலையான முழு irqchip_aplic_update_idc_map(மாறிலி வெறுமை *fdt, முழு nodeoff,
					கட்டமைப்பு aplic_data *pd)
{
	முழு i, err, மொத்தம், cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	மாறிலி fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &மொத்தம்);
	எனில் (!val || மொத்தம் < அளவு(fdt32_t))
		திரும்பு SBI_EINVAL;
	மொத்தம் = மொத்தம் / அளவு(fdt32_t);

	க்கு (i = 0; i < மொத்தம்; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	திரும்பு 0;
}

நிலையான முழு irqchip_aplic_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு aplic_data *pd;

	pd = sbi_zalloc(அளவு(*pd));
	எனில் (!pd)
		திரும்பு SBI_ENOMEM;

	rc = fdt_parse_aplic_முனை(fdt, nodeoff, pd);
	எனில் (rc)
		செல் fail_free_data;

	எனில் (pd->இலக்கம்_idc) {
		pd->idc_map = sbi_zalloc(அளவு(*pd->idc_map) * pd->இலக்கம்_idc);
		எனில் (!pd->idc_map) {
			rc = SBI_ENOMEM;
			செல் fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		எனில் (rc)
			செல் fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	எனில் (rc)
		செல் fail_free_idc_map;

	திரும்பு 0;

fail_free_idc_map:
	எனில் (pd->இலக்கம்_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
