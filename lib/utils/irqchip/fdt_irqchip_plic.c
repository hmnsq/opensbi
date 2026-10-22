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
					   بنية plic_data *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحيح i, err, کيراءن, cpu_offset, cpu_intc_offset;

	لكل (i = 0; i < sbi_hart_کيراءن(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &کيراءن);
	إذا (!val || کيراءن < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	کيراءن = کيراءن / حجم(fdt32_t);

	لكل (i = 0; i < کيراءن; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_نود_offset_by_phandle(fdt, phandle);
		إذا (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		إذا (cpu_offset < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		إذا (err)
			استمر;

		hartindex = sbi_hartid_to_hartindex(hartid);
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
				  ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_کيراءن()));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_plic_نود(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	إذا (rc)
		اذهب fail_free_data;

	أرجع 0;

fail_free_data:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (فراغ *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت بنية fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
