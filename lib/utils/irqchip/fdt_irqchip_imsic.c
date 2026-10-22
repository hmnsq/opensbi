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
#include <sbi_utils/irqchip/imsic.h>

static số_nguyên irqchip_imsic_update_hartid_table(const void *fdt, số_nguyên nodeoff,
					     struct imsic_data *id)
{
	const fdt32_t *val;
	u32 phandle, hwirq, hartid;
	số_nguyên i, err, count, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	nếu (!val || count < sizeof(fdt32_t))
		trả_về SBI_EINVAL;
	count = count / sizeof(fdt32_t);

	cho (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		nếu (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		nếu (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		nếu (err)
			trả_về SBI_EINVAL;

		switch (hwirq) {
		case IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			nếu (err)
				trả_về err;
			break;
		default:
			break;
		}
	}

	trả_về 0;
}

static số_nguyên irqchip_imsic_cold_init(const void *fdt, số_nguyên nodeoff,
				   const struct fdt_match *match)
{
	số_nguyên rc;
	struct imsic_data *id;

	id = sbi_zalloc(sizeof(*id));
	nếu (!id)
		trả_về SBI_ENOMEM;

	rc = fdt_parse_imsic_node(fdt, nodeoff, id);
	nếu (rc || !id->targets_mmode)
		goto fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	nếu (rc)
		goto fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	nếu (rc)
		goto fail_free_data;

	trả_về 0;

fail_free_data:
	sbi_free(id);
	trả_về rc;
}

static const struct fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

const struct fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
