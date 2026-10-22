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
	صحيح i, err, count, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	אם (!val || count < حجم(fdt32_t))
		החזר SBI_EINVAL;
	count = count / حجم(fdt32_t);

	עבור (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		אם (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		אם (cpu_offset < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		אם (err)
			החזר SBI_EINVAL;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			אם (err)
				החזר err;
			اكسر;
		افتراضي:
			اكسر;
		}
	}

	החזר 0;
}

ساكن صحيح irqchip_imsic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية imsic_data *id;

	id = sbi_zalloc(حجم(*id));
	אם (!id)
		החזר SBI_ENOMEM;

	rc = fdt_parse_imsic_node(fdt, nodeoff, id);
	אם (rc || !id->targets_mmode)
		اذهب fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	אם (rc)
		اذهب fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	אם (rc)
		اذهب fail_free_data;

	החזר 0;

fail_free_data:
	sbi_free(id);
	החזר rc;
}

ساكن ثابت بنية fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
