/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/ipi/aclint_mswi.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ipi_mswi_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned long offset;
	ସଂରଚନା aclint_mswi_data *ms;

	ms = sbi_zalloc(ଆକାର(*ms));
	ଯଦି (!ms)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	rc = fdt_parse_aclint_ଗଣ୍ଠି(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, ଖାଲି, ଖାଲି,
				   &ms->first_hartid, &ms->hart_ଗଣନା);
	ଯଦି (rc) {
		sbi_free(ms);
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		ଯଦି ((ms->size - offset) < ACLINT_MSWI_SIZE)
			ଫେରନ୍ତୁ SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	ଯଦି (rc) {
		sbi_free(ms);
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ unsigned long clint_offset = CLINT_MSWI_OFFSET;

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
