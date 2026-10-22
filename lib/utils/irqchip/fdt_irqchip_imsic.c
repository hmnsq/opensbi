/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/irqchip/fdt_irqchip.h>
#ضمن <sbi_utils/irqchip/imsic.h>

ساكن صحيح irqchip_imsic_update_hartid_table(ثابت فراغ *fdt, صحيح nodeoff,
					     بنية imsic_data *id)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid;
	صحيح i, err, تعداد_2, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &تعداد_2);
	إذا (!val || تعداد_2 < حجم(fdt32_t))
		إرجاع SBI_EINVAL;
	تعداد_2 = تعداد_2 / حجم(fdt32_t);

	لكل (i = 0; i < تعداد_2; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_عقدة_offset_by_phandle(fdt, phandle);
		إذا (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		إذا (cpu_offset < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		إذا (err)
			إرجاع SBI_EINVAL;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			إذا (err)
				إرجاع err;
			اكسر;
		افتراضي:
			اكسر;
		}
	}

	إرجاع 0;
}

ساكن صحيح irqchip_imsic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية imsic_data *id;

	id = sbi_zalloc(حجم(*id));
	إذا (!id)
		إرجاع SBI_ENOMEM;

	rc = fdt_parse_imsic_عقدة(fdt, nodeoff, id);
	إذا (rc || !id->targets_mmode)
		اذهب fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	إذا (rc)
		اذهب fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	إذا (rc)
		اذهب fail_free_data;

	إرجاع 0;

fail_free_data:
	sbi_free(id);
	إرجاع rc;
}

ساكن ثابت بنية fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
