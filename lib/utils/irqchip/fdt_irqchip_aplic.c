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

static ចំនួនគត់ irqchip_aplic_update_idc_map(const void *fdt, ចំនួនគត់ nodeoff,
					sa_nracanaa aplic_data *pd)
{
	ចំនួនគត់ i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	const fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	ប្រសិនបើ (!val || count < parimaa_na(fdt32_t))
		ត្រឡប់ SBI_EINVAL;
	count = count / parimaa_na(fdt32_t);

	សម្រាប់ (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ប្រសិនបើ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ប្រសិនបើ (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ប្រសិនបើ (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ប្រសិនបើ (hartindex == -1U)
			continue;

		pd->idc_map[i / 2] = hartindex;
	}

	ត្រឡប់ 0;
}

static ចំនួនគត់ irqchip_aplic_cold_init(const void *fdt, ចំនួនគត់ nodeoff,
				   const sa_nracanaa fdt_match *match)
{
	ចំនួនគត់ rc;
	sa_nracanaa aplic_data *pd;

	pd = sbi_zalloc(parimaa_na(*pd));
	ប្រសិនបើ (!pd)
		ត្រឡប់ SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	ប្រសិនបើ (rc)
		goto fail_free_data;

	ប្រសិនបើ (pd->num_idc) {
		pd->idc_map = sbi_zalloc(parimaa_na(*pd->idc_map) * pd->num_idc);
		ប្រសិនបើ (!pd->idc_map) {
			rc = SBI_ENOMEM;
			goto fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ប្រសិនបើ (rc)
			goto fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ប្រសិនបើ (rc)
		goto fail_free_idc_map;

	ត្រឡប់ 0;

fail_free_idc_map:
	ប្រសិនបើ (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	ត្រឡប់ rc;
}

static const sa_nracanaa fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

const sa_nracanaa fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
