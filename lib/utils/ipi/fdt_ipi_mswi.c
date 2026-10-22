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

স্থির পূর্ণসংখ্যা ipi_mswi_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      ধ্রুবক কাঠামো fdt_match *match)
{
	পূর্ণসংখ্যা rc;
	unsigned long offset;
	কাঠামো aclint_mswi_তথ্য *ms;

	ms = sbi_zalloc(আকার(*ms));
	যদি (!ms)
		ফেরত SBI_ENOMEM;

	rc = fdt_parse_aclint_গ্রন্থি(fdt, nodeoff, false, false,
				   &ms->addr, &ms->আকার_2, ফাঁকা, ফাঁকা,
				   &ms->first_hartid, &ms->hart_গুনতি);
	যদি (rc) {
		sbi_free(ms);
		ফেরত rc;
	}

	যদি (match->তথ্য) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->তথ্য);
		ms->addr += offset;
		যদি ((ms->আকার_2 - offset) < ACLINT_MSWI_আকার)
			ফেরত SBI_EINVAL;
		ms->আকার_2 = ACLINT_MSWI_আকার;
	}

	rc = aclint_mswi_cold_init(ms);
	যদি (rc) {
		sbi_free(ms);
		ফেরত rc;
	}

	ফেরত 0;
}

স্থির ধ্রুবক unsigned long clint_offset = CLINT_MSWI_OFFSET;

স্থির ধ্রুবক কাঠামো fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .তথ্য = &clint_offset },
	{ .compatible = "sifive,clint0", .তথ্য = &clint_offset },
	{ .compatible = "thead,c900-clint", .তথ্য = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
