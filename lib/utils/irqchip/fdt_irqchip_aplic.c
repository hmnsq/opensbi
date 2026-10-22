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

static целое irqchip_aplic_update_idc_map(const void *fdt, целое nodeoff,
					struct aplic_data *pd)
{
	целое i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	const fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	если (!val || count < sizeof(fdt32_t))
		вернуть SBI_EINVAL;
	count = count / sizeof(fdt32_t);

	для (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		если (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		если (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		если (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		если (hartindex == -1U)
			continue;

		pd->idc_map[i / 2] = hartindex;
	}

	вернуть 0;
}

static целое irqchip_aplic_cold_init(const void *fdt, целое nodeoff,
				   const struct fdt_match *match)
{
	целое rc;
	struct aplic_data *pd;

	pd = sbi_zalloc(sizeof(*pd));
	если (!pd)
		вернуть SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	если (rc)
		goto fail_free_data;

	если (pd->num_idc) {
		pd->idc_map = sbi_zalloc(sizeof(*pd->idc_map) * pd->num_idc);
		если (!pd->idc_map) {
			rc = SBI_ENOMEM;
			goto fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		если (rc)
			goto fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	если (rc)
		goto fail_free_idc_map;

	вернуть 0;

fail_free_idc_map:
	если (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	вернуть rc;
}

static const struct fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

const struct fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
