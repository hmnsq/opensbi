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
					بنية aplic_داده *pd)
{
	صحيح i, err, شمار, cpu_جابه_جایی, cpu_intc_جابه_جایی;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمار);
	إذا (!val || شمار < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	شمار = شمار / حجم(fdt32_t);

	لكل (i = 0; i < شمار; i += 2) {
		phandle = fdt32_مقصد_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	أرجع 0;
}

ساكن صحيح irqchip_aplic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_تطابق *تطابق)
{
	صحيح rc;
	بنية aplic_داده *pd;

	pd = sbi_zalloc(حجم(*pd));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aplic_گره(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_داده;

	إذا (pd->رقم_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->رقم_idc);
		إذا (!pd->idc_map) {
			rc = SBI_ENOMEM;
			اذهب fail_free_داده;
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
	إذا (pd->رقم_idc)
		sbi_free(pd->idc_map);
fail_free_داده:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_تطابق irqchip_aplic_تطابق[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_aplic = {
	.تطابق_جدول = irqchip_aplic_تطابق,
	.init = irqchip_aplic_cold_init,
};
