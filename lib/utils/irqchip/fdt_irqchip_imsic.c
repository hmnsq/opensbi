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

static puur_naa_nka irqchip_imsic_update_hartid_table(const void *fdt, puur_naa_nka nodeoff,
					     sa_nracanaa imsic_data *id)
{
	const fdt32_t *val;
	u32 phandle, hwirq, hartid;
	puur_naa_nka i, err, count, cpu_offset, cpu_intc_offset;

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	ᨀᨘ (!val || count < parimaa_na(fdt32_t))
		ᨀᨘᨒ SBI_EINVAL;
	count = count / parimaa_na(fdt32_t);

	for (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ᨀᨘ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ᨀᨘ (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᨀᨘ (err)
			ᨀᨘᨒ SBI_EINVAL;

		switch (hwirq) {
		pak_sa IRQ_M_EXT:
			err = imsic_map_hartid_to_data(hartid, id, i / 2);
			ᨀᨘ (err)
				ᨀᨘᨒ err;
			bha_nga;
		default:
			bha_nga;
		}
	}

	ᨀᨘᨒ 0;
}

static puur_naa_nka irqchip_imsic_cold_init(const void *fdt, puur_naa_nka nodeoff,
				   const sa_nracanaa fdt_match *match)
{
	puur_naa_nka rc;
	sa_nracanaa imsic_data *id;

	id = sbi_zalloc(parimaa_na(*id));
	ᨀᨘ (!id)
		ᨀᨘᨒ SBI_ENOMEM;

	rc = fdt_parse_imsic_node(fdt, nodeoff, id);
	ᨀᨘ (rc || !id->targets_mmode)
		goto fail_free_data;

	rc = imsic_cold_irqchip_init(id);
	ᨀᨘ (rc)
		goto fail_free_data;

	rc = irqchip_imsic_update_hartid_table(fdt, nodeoff, id);
	ᨀᨘ (rc)
		goto fail_free_data;

	ᨀᨘᨒ 0;

fail_free_data:
	sbi_free(id);
	ᨀᨘᨒ rc;
}

static const sa_nracanaa fdt_match irqchip_imsic_match[] = {
	{ .compatible = "riscv,imsics" },
	{ },
};

const sa_nracanaa fdt_driver fdt_irqchip_imsic = {
	.match_table = irqchip_imsic_match,
	.init = irqchip_imsic_cold_init,
};
