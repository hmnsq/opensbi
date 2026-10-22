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
	صحيح i, err, شمېر, cpu_دننه_واټن, cpu_intc_دننه_واټن;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	شمېر = شمېر / حجم(fdt32_t);

	لكل (i = 0; i < شمېر; i += 2) {
		phandle = fdt32_منزل_cpu(val[i]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
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
				   ثابت بنية fdt_سمون *سمون)
{
	صحيح rc;
	بنية aplic_معلومات *pd;

	pd = sbi_zalloc(حجم(*pd));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aplic_غوټه(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_معلومات;

	إذا (pd->عدد_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->عدد_idc);
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
	إذا (pd->عدد_idc)
		sbi_free(pd->idc_map);
fail_free_معلومات:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_سمون irqchip_aplic_سمون[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_aplic = {
	.سمون_جدول = irqchip_aplic_سمون,
	.init = irqchip_aplic_cold_init,
};
