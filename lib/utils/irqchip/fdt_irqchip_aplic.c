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

static 정수 irqchip_aplic_update_idc_map(const void *fdt, 정수 nodeoff,
					struct aplic_data *pd)
{
	정수 i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	const fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	만약 (!val || count < sizeof(fdt32_t))
		반환 SBI_EINVAL;
	count = count / sizeof(fdt32_t);

	위해 (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		만약 (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		만약 (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		만약 (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		만약 (hartindex == -1U)
			continue;

		pd->idc_map[i / 2] = hartindex;
	}

	반환 0;
}

static 정수 irqchip_aplic_cold_init(const void *fdt, 정수 nodeoff,
				   const struct fdt_match *match)
{
	정수 rc;
	struct aplic_data *pd;

	pd = sbi_zalloc(sizeof(*pd));
	만약 (!pd)
		반환 SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	만약 (rc)
		goto fail_free_data;

	만약 (pd->num_idc) {
		pd->idc_map = sbi_zalloc(sizeof(*pd->idc_map) * pd->num_idc);
		만약 (!pd->idc_map) {
			rc = SBI_ENOMEM;
			goto fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		만약 (rc)
			goto fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	만약 (rc)
		goto fail_free_idc_map;

	반환 0;

fail_free_idc_map:
	만약 (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	반환 rc;
}

static const struct fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

const struct fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
