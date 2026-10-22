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

static 整数 ipi_mswi_cold_init(const void *fdt, 整数 nodeoff,
			      const struct fdt_match *match)
{
	整数 rc;
	unsigned long offset;
	struct aclint_mswi_data *ms;

	ms = sbi_zalloc(sizeof(*ms));
	如果 (!ms)
		返回 SBI_ENOMEM;

	rc = fdt_parse_aclint_node(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, NULL, NULL,
				   &ms->first_hartid, &ms->hart_count);
	如果 (rc) {
		sbi_free(ms);
		返回 rc;
	}

	如果 (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		如果 ((ms->size - offset) < ACLINT_MSWI_SIZE)
			返回 SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	如果 (rc) {
		sbi_free(ms);
		返回 rc;
	}

	返回 0;
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
