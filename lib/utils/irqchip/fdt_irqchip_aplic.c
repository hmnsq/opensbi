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

static จำนวนเต็ม irqchip_aplic_update_idc_map(const void *fdt, จำนวนเต็ม nodeoff,
					sa_nracanaa aplic_data *pd)
{
	จำนวนเต็ม i, err, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hartid, hartindex;
	const fdt32_t *val;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	ถ้า (!val || count < parimaa_na(fdt32_t))
		คืน SBI_EINVAL;
	count = count / parimaa_na(fdt32_t);

	สำหรับ (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ถ้า (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ถ้า (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ถ้า (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		ถ้า (hartindex == -1U)
			continue;

		pd->idc_map[i / 2] = hartindex;
	}

	คืน 0;
}

static จำนวนเต็ม irqchip_aplic_cold_init(const void *fdt, จำนวนเต็ม nodeoff,
				   const sa_nracanaa fdt_match *match)
{
	จำนวนเต็ม rc;
	sa_nracanaa aplic_data *pd;

	pd = sbi_zalloc(parimaa_na(*pd));
	ถ้า (!pd)
		คืน SBI_ENOMEM;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	ถ้า (rc)
		goto fail_free_data;

	ถ้า (pd->num_idc) {
		pd->idc_map = sbi_zalloc(parimaa_na(*pd->idc_map) * pd->num_idc);
		ถ้า (!pd->idc_map) {
			rc = SBI_ENOMEM;
			goto fail_free_data;
		}

		rc = irqchip_aplic_update_idc_map(fdt, nodeoff, pd);
		ถ้า (rc)
			goto fail_free_idc_map;
	}

	rc = aplic_cold_irqchip_init(pd);
	ถ้า (rc)
		goto fail_free_idc_map;

	คืน 0;

fail_free_idc_map:
	ถ้า (pd->num_idc)
		sbi_free(pd->idc_map);
fail_free_data:
	sbi_free(pd);
	คืน rc;
}

static const sa_nracanaa fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

const sa_nracanaa fdt_driver fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.init = irqchip_aplic_cold_init,
};
