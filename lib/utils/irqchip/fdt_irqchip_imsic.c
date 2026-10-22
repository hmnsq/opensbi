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

ساکن صحیح irqchip_imsic_update_hartid_جدول(ثابت خالی *fdt, صحیح nodeoff,
					     ساخت imsic_ڈیٹا *id)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid;
	صحیح i, err, گنتی, cpu_انحراف, cpu_intc_انحراف;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &گنتی);
	اگر (!val || گنتی < حجم(fdt32_t))
		واپس SBI_EINVAL;
	گنتی = گنتی / حجم(fdt32_t);

	برائے (i = 0; i < گنتی; i += 2) {
		phandle = fdt32_مقصد_cpu(val[i]);
		hwirq = fdt32_مقصد_cpu(val[i + 1]);

		cpu_intc_انحراف = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
		اگر (cpu_intc_انحراف < 0)
			جاری;

		cpu_انحراف = fdt_والد_انحراف(fdt, cpu_intc_انحراف);
		اگر (cpu_انحراف < 0)
			جاری;

		err = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (err)
			واپس SBI_EINVAL;

		انتخاب (hwirq) {
		حالت IRQ_M_EXT:
			err = imsic_map_hartid_مقصد_ڈیٹا(hartid, id, i / 2);
			اگر (err)
				واپس err;
			توڑو;
		طےشدہ:
			توڑو;
		}
	}

	واپس 0;
}

ساکن صحیح irqchip_imsic_cold_init(ثابت خالی *fdt, صحیح nodeoff,
				   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت imsic_ڈیٹا *id;

	id = sbi_zalloc(حجم(*id));
	اگر (!id)
		واپس SBI_ENOMEM;

	rc = fdt_parse_imsic_گرہ(fdt, nodeoff, id);
	اگر (rc || !id->targets_mmode)
		جاؤ fail_free_ڈیٹا;

	rc = imsic_cold_irqchip_init(id);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	rc = irqchip_imsic_update_hartid_جدول(fdt, nodeoff, id);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	واپس 0;

fail_free_ڈیٹا:
	sbi_free(id);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت irqchip_imsic_مطابقت[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت ساخت fdt_driver fdt_irqchip_imsic = {
	.مطابقت_جدول = irqchip_imsic_مطابقت,
	.init = irqchip_imsic_cold_init,
};
