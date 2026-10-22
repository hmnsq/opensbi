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
					     بنية imsic_معلومات *id)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid;
	صحيح i, err, شمېر, cpu_دننه_واټن, cpu_intc_دننه_واټن;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	شمېر = شمېر / حجم(fdt32_t);

	لكل (i = 0; i < شمېر; i += 2) {
		phandle = fdt32_منزل_cpu(val[i]);
		hwirq = fdt32_منزل_cpu(val[i + 1]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (err)
			أرجع SBI_EINVAL;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			err = imsic_map_hartid_منزل_معلومات(hartid, id, i / 2);
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
				   ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية imsic_معلومات *id;

	id = sbi_zalloc(حجم(*id));
	إذا (!id)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_imsic_غوټه(fdt, nodeoff, id);
	إذا (rc || !id->targets_mmode)
		اذهب fail_free_معلومات;

	rc = imsic_cold_irqchip_init(id);
	إذا (rc)
		اذهب fail_free_معلومات;

	rc = irqchip_imsic_update_hartid_جدول(fdt, nodeoff, id);
	إذا (rc)
		اذهب fail_free_معلومات;

	أرجع 0;

fail_free_معلومات:
	sbi_free(id);
	أرجع rc;
}

ساكن ثابت بنية fdt_سمون irqchip_imsic_سمون[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_imsic = {
	.سمون_جدول = irqchip_imsic_سمون,
	.init = irqchip_imsic_cold_init,
};
