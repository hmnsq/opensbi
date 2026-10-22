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

ساكن صحيح irqchip_imsic_update_hartid_جدول(ثابت فراغ *fdt, صحيح nodeoff,
					     بنية imsic_داده *id)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid;
	صحيح i, err, شمار, cpu_جابه_جایی, cpu_intc_جابه_جایی;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمار);
	إذا (!val || شمار < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	شمار = شمار / حجم(fdt32_t);

	لكل (i = 0; i < شمار; i += 2) {
		phandle = fdt32_مقصد_cpu(val[i]);
		hwirq = fdt32_مقصد_cpu(val[i + 1]);

		cpu_intc_جابه_جایی = fdt_گره_جابه_جایی_by_phandle(fdt, phandle);
		إذا (cpu_intc_جابه_جایی < 0)
			استمر;

		cpu_جابه_جایی = fdt_والد_جابه_جایی(fdt, cpu_intc_جابه_جایی);
		إذا (cpu_جابه_جایی < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);
		إذا (err)
			أرجع SBI_EINVAL;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			err = imsic_map_hartid_مقصد_داده(hartid, id, i / 2);
			إذا (err)
				أرجع err;
			اكسر;
		افتراضي:
			اكسر;
		}
	}

	أرجع 0;
}

ساكن صحيح irqchip_imsic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية imsic_داده *id;

	id = sbi_zalloc(حجم(*id));
	إذا (!id)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_imsic_گره(fdt, nodeoff, id);
	إذا (rc || !id->targets_mmode)
		اذهب fail_free_داده;

	rc = imsic_cold_irqchip_init(id);
	إذا (rc)
		اذهب fail_free_داده;

	rc = irqchip_imsic_update_hartid_جدول(fdt, nodeoff, id);
	إذا (rc)
		اذهب fail_free_داده;

	أرجع 0;

fail_free_داده:
	sbi_free(id);
	أرجع rc;
}

ساكن ثابت بنية fdt_تطابق irqchip_imsic_تطابق[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_imsic = {
	.تطابق_جدول = irqchip_imsic_تطابق,
	.init = irqchip_imsic_cold_init,
};
