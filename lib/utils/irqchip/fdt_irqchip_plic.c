/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/irqchip/fdt_irqchip.h>
#ضمن <sbi_utils/irqchip/plic.h>

ساكن صحيح irqchip_plic_update_context_map(ثابت فراغ *fdt, صحيح nodeoff,
					   بنية plic_داده *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحيح i, err, شمار, cpu_جابه_جایی, cpu_intc_جابه_جایی;

	لكل (i = 0; i < sbi_hart_شمار(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			استمر;

		hartindex = sbi_hartid_مقصد_hartindex(hartid);
		إذا (hartindex == -1U)
			استمر;

		بدل (hwirq) {
		حالة IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			اكسر;
		حالة IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			اكسر;
		}
	}

	أرجع 0;
}

ساكن صحيح irqchip_plic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				  ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية plic_داده *pd;

	pd = sbi_zalloc(PLIC_داده_اندازه(sbi_hart_شمار()));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_plic_گره(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_داده;

	pd->flags = (بیعلامت دراز)تطابق->داده;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_داده;

	rc = plic_cold_irqchip_init(pd);
	إذا (rc)
		اذهب fail_free_داده;

	أرجع 0;

fail_free_داده:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_تطابق irqchip_plic_تطابق[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .داده = (فراغ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_irqchip_plic = {
	.تطابق_جدول = irqchip_plic_تطابق,
	.init = irqchip_plic_cold_init,
};
