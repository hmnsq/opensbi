/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/ipi/aclint_mswi.h>

স্থির পূর্ণাঙ্ক ipi_mswi_cold_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুব সংরচনা fdt_match *match)
{
	পূর্ণাঙ্ক rc;
	unsigned long offset;
	সংরচনা aclint_mswi_data *ms;

	ms = sbi_zalloc(পরিমাণ(*ms));
	যদি (!ms)
		প্রত্যাগম SBI_ENOMEM;

	rc = fdt_parse_aclint_গ্রন্থি(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, রিক্ত, রিক্ত,
				   &ms->first_hartid, &ms->hart_গুনতি);
	যদি (rc) {
		sbi_free(ms);
		প্রত্যাগম rc;
	}

	যদি (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		যদি ((ms->size - offset) < ACLINT_MSWI_SIZE)
			প্রত্যাগম SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	যদি (rc) {
		sbi_free(ms);
		প্রত্যাগম rc;
	}

	প্রত্যাগম 0;
}

স্থির ধ্রুব unsigned long clint_offset = CLINT_MSWI_OFFSET;

স্থির ধ্রুব সংরচনা fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
