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
#அந்தர்பாவ <sbi_utils/irqchip/imsic.h>

நிலையான முழு irqchip_imsic_update_hartid_table(மாறிலி வெறுமை *fdt, முழு nodeoff,
					     கட்டமைப்பு imsic_data *id)
{
	மாறிலி fdt32_t *val;
	u32 phandle, hwirq, hartid;
	முழு i, err, மொத்தம், cpu_offset, cpu_intc_offset;

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
			திரும்பு SBI_EINVAL;

		தேர்வு (hwirq) {
		நிலை IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			எனில் (err)
				திரும்பு err;
			நிறுத்து;
		இயல்பு:
			நிறுத்து;
		}
	}

	திரும்பு 0;
}

நிலையான முழு irqchip_imsic_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				   மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	கட்டமைப்பு imsic_data *id;

	id = sbi_zalloc(அளவு(*id));
	எனில் (!id)
		திரும்பு SBI_ENOMEM;

	rc = fdt_parse_imsic_முனை(fdt, nodeoff, id);
	எனில் (rc || !id->targets_mmode)
		செல் fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	எனில் (rc)
		செல் fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	எனில் (rc)
		செல் fail_free_data;

	திரும்பு 0;

fail_free_data:
	sbi_free(id);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
