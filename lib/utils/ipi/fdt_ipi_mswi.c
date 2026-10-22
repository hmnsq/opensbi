/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/ipi/aclint_mswi.h>

સ્થિર પૂર્ણાંક ipi_mswi_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	પૂર્ણાંક rc;
	unsigned long offset;
	સંરચના aclint_mswi_data *ms;

	ms = sbi_zalloc(કદ(*ms));
	જો (!ms)
		પાછા SBI_ENOMEM;

	rc = fdt_parse_aclint_ગાંઠ(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, ખાલી, ખાલી,
				   &ms->first_hartid, &ms->hart_ગણતરી);
	જો (rc) {
		sbi_free(ms);
		પાછા rc;
	}

	જો (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		જો ((ms->size - offset) < ACLINT_MSWI_SIZE)
			પાછા SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	જો (rc) {
		sbi_free(ms);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર અચલ unsigned long clint_offset = CLINT_MSWI_OFFSET;

સ્થિર અચલ સંરચના fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

અચલ સંરચના fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
