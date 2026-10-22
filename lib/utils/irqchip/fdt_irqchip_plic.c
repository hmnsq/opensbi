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

ایستا صحیح irqchip_plic_update_context_map(ثابت تهی *fdt, صحیح nodeoff,
					   ساختار plic_data *pd)
{
	ثابت fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	صحیح i, err, شمار, cpu_offset, cpu_intc_offset;

	برای (i = 0; i < sbi_hart_شمار(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت SBI_EINVAL;
	شمار = شمار / اندازه(fdt32_t);

	برای (i = 0; i < شمار; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_گره_offset_by_phandle(fdt, phandle);
		اگر (cpu_intc_offset < 0)
			ادامه;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		اگر (cpu_offset < 0)
			ادامه;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		اگر (err)
			ادامه;

		hartindex = sbi_hartid_to_hartindex(hartid);
		اگر (hartindex == -1U)
			ادامه;

		انتخاب (hwirq) {
		حالت IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			بشکن;
		حالت IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			بشکن;
		}
	}

	بازگشت 0;
}

ایستا صحیح irqchip_plic_cold_init(ثابت تهی *fdt, صحیح nodeoff,
				  ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_شمار()));
	اگر (!pd)
		بازگشت SBI_ENOMEM;

	rc = fdt_parse_plic_گره(fdt, nodeoff, pd);
	اگر (rc)
		برو fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	اگر (rc)
		برو fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	اگر (rc)
		برو fail_free_data;

	بازگشت 0;

fail_free_data:
	sbi_free(pd);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (تهی *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

ثابت ساختار fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
