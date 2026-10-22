/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/irqchip/fdt_irqchip.h>
#شامل <sbi_utils/irqchip/imsic.h>

ایستا صحیح irqchip_imsic_update_hartid_table(ثابت تهی *fdt, صحیح nodeoff,
					     ساختار imsic_data *id)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid;
	صحیح i, err, شمار, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت SBI_EINVAL;
	شمار = شمار / اندازه(fdt32_t);

	برای (i = 0; i < شمار; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_گره_offset_by_phandle(fdt, phandle);
		اگر (cpu_intc_offset < 0)
			ادامه;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		اگر (cpu_offset < 0)
			ادامه;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (err)
			بازگشت SBI_EINVAL;

		انتخاب (hwirq) {
		حالت IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			اگر (err)
				بازگشت err;
			بشکن;
		پیشفرض:
			بشکن;
		}
	}

	بازگشت 0;
}

ایستا صحیح irqchip_imsic_cold_init(ثابت تهی *fdt, صحیح nodeoff,
				   ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار imsic_data *id;

	id = sbi_zalloc(اندازه(*id));
	اگر (!id)
		بازگشت SBI_ENOMEM;

	rc = fdt_parse_imsic_گره(fdt, nodeoff, id);
	اگر (rc || !id->targets_mmode)
		برو fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	اگر (rc)
		برو fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	اگر (rc)
		برو fail_free_data;

	بازگشت 0;

fail_free_data:
	sbi_free(id);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت ساختار fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
