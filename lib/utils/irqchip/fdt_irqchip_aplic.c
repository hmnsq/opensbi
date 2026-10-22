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
#شامل <sbi_utils/irqchip/aplic.h>

ساکن صحیح irqchip_aplic_update_idc_map(ثابت خالی *fdt, صحیح nodeoff,
					ساخت aplic_ڈیٹا *pd)
{
	صحیح i, err, گنتی, cpu_انحراف, cpu_intc_انحراف;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &گنتی);
	اگر (!val || گنتی < حجم(fdt32_t))
		واپس SBI_EINVAL;
	گنتی = گنتی / حجم(fdt32_t);

	برائے (i = 0; i < گنتی; i += 2) {
		phandle = fdt32_مقصد_cpu(val[i]);

		cpu_intc_انحراف = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
		اگر (cpu_intc_انحراف < 0)
			جاری;

		cpu_انحراف = fdt_والد_انحراف(fdt, cpu_intc_انحراف);
		اگر (cpu_انحراف < 0)
			جاری;

		err = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (err)
			جاری;

		hartindex = sbi_hartid_مقصد_hartindex(hartid);
		اگر (hartindex == -1U)
			جاری;

		pd->idc_map[i / 2] = hartindex;
	}

	واپس 0;
}

ساکن صحیح irqchip_aplic_cold_init(ثابت خالی *fdt, صحیح nodeoff,
				   ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت aplic_ڈیٹا *pd;

	pd = sbi_zalloc(حجم(*pd));
	اگر (!pd)
		واپس SBI_ENOMEM;

	rc = fdt_parse_aplic_گرہ(fdt, nodeoff, pd);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	اگر (pd->عدد_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->عدد_idc);
		اگر (!pd->idc_map) {
			rc = SBI_ENOMEM;
			جاؤ fail_free_ڈیٹا;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		اگر (rc)
			جاؤ fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	اگر (rc)
		جاؤ fail_free_idc_map;

	واپس 0;

fail_free_idc_map:
	اگر (pd->عدد_idc)
		sbi_free(pd->idc_map);
fail_free_ڈیٹا:
	sbi_free(pd);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت irqchip_aplic_مطابقت[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت ساخت fdt_driver fdt_irqchip_aplic = {
	.مطابقت_جدول = irqchip_aplic_مطابقت,
	.init = irqchip_aplic_cold_init,
};
