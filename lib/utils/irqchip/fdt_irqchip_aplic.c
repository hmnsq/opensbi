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
	صحيح i, err, كدي, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	ثابت fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &كدي);
	إذا (!val || كدي < حجم(fdt32_t))
		أرجع SBI_EINVAL;
	كدي = كدي / حجم(fdt32_t);

	لكل (i = 0; i < كدي; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_كلي_offset_by_phandle(fdt, phandle);
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

		pd->idc_map[i / 2] = hartindex;
	}

	أرجع 0;
}

ساكن صحيح irqchip_aplic_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
				   ثابت بنية fdt_match *match)
{
	صحيح rc;
	بنية aplic_data *pd;

	pd = sbi_zalloc(حجم(*pd));
	إذا (!pd)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aplic_كلي(fdt, nodeoff, pd);
	إذا (rc)
		اذهب fail_free_data;

	إذا (pd->لمب_idc) {
		pd->idc_map = sbi_zalloc(حجم(*pd->idc_map) * pd->لمب_idc);
		إذا (!pd->idc_map) {
			rc = SBI_ENOMEM;
			اذهب fail_free_data;
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
	إذا (pd->لمب_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	أرجع rc;
}

ساكن ثابت بنية fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

ثابت بنية fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
