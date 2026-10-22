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
					   بنية plic_معلومات *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحيح i, err, ڳڻپ, cpu_وٿي, cpu_intc_وٿي;

	لكل (i = 0; i < sbi_hart_ڳڻپ(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ڳڻپ);
	إذا (!val || ڳڻپ < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	ڳڻپ = ڳڻپ / حجم(fdt32_t);

	لكل (i = 0; i < ڳڻپ; i += 2) {
		phandle = fdt32_منزل_cpu(val[i]);
		hwirq = fdt32_منزل_cpu(val[i + 1]);

		cpu_intc_وٿي = fdt_ڳنڍ_وٿي_by_phandle(fdt, phandle);
		إذا (cpu_intc_وٿي < 0)
			استمر;

		cpu_وٿي = fdt_والد_وٿي(fdt, cpu_intc_وٿي);
		إذا (cpu_وٿي < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);
		إذا (err)
			استمر;

		hartindex = sbi_hartid_منزل_hartindex(hartid);
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
				  ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية plic_معلومات *pd;

	pd = sbi_zalloc(PLIC_معلومات_سائيز(sbi_hart_ڳڻپ()));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_plic_ڳنڍ(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	pd->flags = (بنانشان ڊگهو)ميلاپ->معلومات;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	rc = plic_cold_irqchip_init(pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	أرجع 0;

fail_free_معلومات:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_ميلاپ irqchip_plic_ميلاپ[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .معلومات = (فراغ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_irqchip_plic = {
	.ميلاپ_جدول = irqchip_plic_ميلاپ,
	.init = irqchip_plic_cold_init,
};
