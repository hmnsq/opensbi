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
					بنية aplic_data *pd)
{
	صحيح i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	אם (!val || count < حجم(fdt32_t))
		החזר SBI_EINVAL;
	count = count / حجم(fdt32_t);

	עבור (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		אם (cpu_intc_offset < 0)
			استمر;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		אם (cpu_offset < 0)
			استمر;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		אם (err)
			استمر;

		hartindex = sbi_hartid_to_hartindex(hartid);
		אם (hartindex == -1U)
			استمر;

		pd->idc_map[i / 2] = hartindex;
	}

	החזר 0;
}

ساكن صحيح irqchip_aplic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية aplic_data *pd;

	pd = sbi_zalloc(حجم(*pd));
	אם (!pd)
		החזר SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	אם (rc)
		اذهب fail_free_data;

	אם (pd->num_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->num_idc);
		אם (!pd->idc_map) {
			rc = SBI_ENOMEM;
			اذهب fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		אם (rc)
			اذهب fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	אם (rc)
		اذهب fail_free_idc_map;

	החזר 0;

fail_free_idc_map:
	אם (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	החזר rc;
}

ساكن ثابت بنية fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
