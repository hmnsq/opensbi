/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/irqchip/fdt_irqchip.h>
#شامل <sbi_utils/irqchip/plic.h>

ساکن صحیح irqchip_plic_update_context_map(ثابت خالی *fdt, صحیح nodeoff,
					   ساخت plic_ڈیٹا *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحیح i, err, گنتی, cpu_انحراف, cpu_intc_انحراف;

	برائے (i = 0; i < sbi_hart_گنتی(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

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
			جاری;

		hartindex = sbi_hartid_مقصد_hartindex(hartid);
		اگر (hartindex == -1U)
			جاری;

		انتخاب (hwirq) {
		حالت IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			توڑو;
		حالت IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			توڑو;
		}
	}

	واپس 0;
}

ساکن صحیح irqchip_plic_cold_init(ثابت خالی *fdt, صحیح nodeoff,
				  ثابت ساخت fdt_مطابقت *مطابقت)
{
	صحیح rc;
	ساخت plic_ڈیٹا *pd;

	pd = sbi_zalloc(PLIC_ڈیٹا_سائز(sbi_hart_گنتی()));
	اگر (!pd)
		واپس SBI_ENOMEM;

	rc = fdt_parse_plic_گرہ(fdt, nodeoff, pd);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	pd->flags = (غیرعلامتی طویل)مطابقت->ڈیٹا;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	rc = plic_cold_irqchip_init(pd);
	اگر (rc)
		جاؤ fail_free_ڈیٹا;

	واپس 0;

fail_free_ڈیٹا:
	sbi_free(pd);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت irqchip_plic_مطابقت[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .ڈیٹا = (خالی *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت ساخت fdt_driver fdt_irqchip_plic = {
	.مطابقت_جدول = irqchip_plic_مطابقت,
	.init = irqchip_plic_cold_init,
};
