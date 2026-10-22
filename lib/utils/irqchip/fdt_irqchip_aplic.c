/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/irqchip/aplic.h>

static පූර්ණ_සංඛ්‍යා irqchip_aplic_update_idc_map(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
					sa_nracanaa aplic_data *pd)
{
	පූර්ණ_සංඛ්‍යා i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	const fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	නම් (!val || count < parimaa_na(fdt32_t))
		ආපසු SBI_EINVAL;
	count = count / parimaa_na(fdt32_t);

	සඳහා (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		නම් (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		නම් (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		නම් (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		නම් (hartindex == -1U)
			continue;

		pd->idc_map[i / 2] = hartindex;
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා irqchip_aplic_cold_init(const void *fdt, පූර්ණ_සංඛ්‍යා nodeoff,
				   const sa_nracanaa fdt_match *match)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa aplic_data *pd;

	pd = sbi_zalloc(parimaa_na(*pd));
	නම් (!pd)
		ආපසු SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	නම් (rc)
		goto fail_free_data;

	නම් (pd->num_idc) {
		pd->idc_map = sbi_zalloc(parimaa_na(*pd->idc_map) * pd->num_idc);
		නම් (!pd->idc_map) {
			rc = SBI_ENOMEM;
			goto fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		නම් (rc)
			goto fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	නම් (rc)
		goto fail_free_idc_map;

	ආපසු 0;

fail_free_idc_map:
	නම් (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	ආපසු rc;
}

static const sa_nracanaa fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

const sa_nracanaa fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
