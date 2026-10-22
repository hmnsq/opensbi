/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/fdt/fdt_driver.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/ipi/aclint_mswi.h>

static số_nguyên ipi_mswi_cold_init(const void *fdt, số_nguyên nodeoff,
			      const struct fdt_match *match)
{
	số_nguyên rc;
	unsigned long offset;
	struct aclint_mswi_data *ms;

	ms = sbi_zalloc(sizeof(*ms));
	nếu (!ms)
		trả_về SBI_ENOMEM;

	rc = fdt_parse_aclint_node(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, NULL, NULL,
				   &ms->first_hartid, &ms->hart_count);
	nếu (rc) {
		sbi_free(ms);
		trả_về rc;
	}

	nếu (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		nếu ((ms->size - offset) < ACLINT_MSWI_SIZE)
			trả_về SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	nếu (rc) {
		sbi_free(ms);
		trả_về rc;
	}

	trả_về 0;
}

static const unsigned long clint_offset = CLINT_MSWI_OFFSET;

static const struct fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

const struct fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
