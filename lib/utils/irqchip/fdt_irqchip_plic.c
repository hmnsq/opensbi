/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_scratch.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/irqchip/plic.h>

static 整数 irqchip_plic_update_context_map(const void *fdt, 整数 nodeoff,
					   struct plic_data *pd)
{
	const fdt32_t *val;
	u32 phandle, hwirq, hartid, hartindex;
	整数 i, err, count, cpu_offset, cpu_intc_offset;

	ために (i = 0; i < sbi_hart_count(); i++) {
		pd->context_map[i][PLIC_M_CONTEXT] = -1;
		pd->context_map[i][PLIC_S_CONTEXT] = -1;
	}

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &count);
	もし (!val || count < sizeof(fdt32_t))
		戻る SBI_EINVAL;
	count = count / sizeof(fdt32_t);

	ために (i = 0; i < count; i += 2) {
		phandle = fdt32_to_cpu(val[i]);
		hwirq = fdt32_to_cpu(val[i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		もし (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		もし (cpu_offset < 0)
			continue;

		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		もし (err)
			continue;

		hartindex = sbi_hartid_to_hartindex(hartid);
		もし (hartindex == -1U)
			continue;

		switch (hwirq) {
		case IRQ_M_EXT:
			pd->context_map[hartindex][PLIC_M_CONTEXT] = i / 2;
			break;
		case IRQ_S_EXT:
			pd->context_map[hartindex][PLIC_S_CONTEXT] = i / 2;
			break;
		}
	}

	戻る 0;
}

static 整数 irqchip_plic_cold_init(const void *fdt, 整数 nodeoff,
				  const struct fdt_match *match)
{
	整数 rc;
	struct plic_data *pd;

	pd = sbi_zalloc(PLIC_DATA_SIZE(sbi_hart_count()));
	もし (!pd)
		戻る SBI_ENOMEM;

	rc = fdt_parse_plic_node(fdt, nodeoff, pd);
	もし (rc)
		goto fail_free_data;

	pd->flags = (unsigned long)match->data;

	rc = irqchip_plic_update_context_map(fdt, nodeoff, pd);
	もし (rc)
		goto fail_free_data;

	rc = plic_cold_irqchip_init(pd);
	もし (rc)
		goto fail_free_data;

	戻る 0;

fail_free_data:
	sbi_free(pd);
	戻る rc;
}

static const struct fdt_match irqchip_plic_match[] = {
	{ .compatible = "andestech,nceplic100" },
	{ .compatible = "riscv,plic0" },
	{ .compatible = "sifive,plic-1.0.0" },
	{ .compatible = "thead,c900-plic",
	  .data = (void *)(PLIC_FLAG_THEAD_DELEGATION | PLIC_FLAG_ENABLE_PM) },
	{ /* sentinel */ }
};

const struct fdt_driver fdt_irqchip_plic = {
	.match_table = irqchip_plic_match,
	.init = irqchip_plic_cold_init,
};
