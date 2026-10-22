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
#ضمن <sbi_utils/irqchip/aplic.h>

ساكن صحيح irqchip_aplic_update_idc_map(ثابت فراغ *fdt, صحيح nodeoff,
					بنية aplic_معلومات *pd)
{
	صحيح i, err, ڳڻپ, cpu_وٿي, cpu_intc_وٿي;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &ڳڻپ);
	إذا (!val || ڳڻپ < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	ڳڻپ = ڳڻپ / حجم(fdt32_t);

	لكل (i = 0; i < ڳڻپ; i += 2) {
		phandle = fdt32_منزل_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	أرجع 0;
}

ساكن صحيح irqchip_aplic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_ميلاپ *ميلاپ)
{
	صحيح rc;
	بنية aplic_معلومات *pd;

	pd = sbi_zalloc(حجم(*pd));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aplic_ڳنڍ(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	إذا (pd->انگ_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->انگ_idc);
		إذا (!pd->idc_map) {
			rc = SBI_ENOMEM;
			اذهب fail_free_معلومات;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		إذا (rc)
			اذهب fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	إذا (rc)
		اذهب fail_free_idc_map;

	أرجع 0;

fail_free_idc_map:
	إذا (pd->انگ_idc)
		sbi_free(pd->idc_map);
fail_free_معلومات:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_ميلاپ irqchip_aplic_ميلاپ[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_aplic = {
	.ميلاپ_جدول = irqchip_aplic_ميلاپ,
	.init = irqchip_aplic_cold_init,
};
