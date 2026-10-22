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

ایستا صحیح irqchip_aplic_update_idc_map(ثابت تهی *fdt, صحیح nodeoff,
					ساختار aplic_data *pd)
{
	صحیح i, err, شمار, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &شمار);
	اگر (!val || شمار < اندازه(fdt32_t))
		بازگشت SBI_EINVAL;
	شمار = شمار / اندازه(fdt32_t);

	برای (i = 0; i < شمار; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

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

		pd->idc_map[i / 2] = hartindex;
	}

	بازگشت 0;
}

ایستا صحیح irqchip_aplic_cold_init(ثابت تهی *fdt, صحیح nodeoff,
				   ثابت ساختار fdt_match *match)
{
	صحیح rc;
	ساختار aplic_data *pd;

	pd = sbi_zalloc(اندازه(*pd));
	اگر (!pd)
		بازگشت SBI_ENOMEM;

	rc = fdt_parse_aplic_گره(fdt, nodeoff, pd);
	اگر (rc)
		برو fail_free_data;

	اگر (pd->رقم_idc) {
		pd->idc_map = sbi_zalloc(اندازه(*pd->idc_map) * pd->رقم_idc);
		اگر (!pd->idc_map) {
			rc = SBI_ENOMEM;
			برو fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		اگر (rc)
			برو fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	اگر (rc)
		برو fail_free_idc_map;

	بازگشت 0;

fail_free_idc_map:
	اگر (pd->رقم_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت ساختار fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
