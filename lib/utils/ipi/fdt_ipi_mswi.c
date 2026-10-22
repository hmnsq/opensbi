/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_driver.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/ipi/aclint_mswi.h>

ایستا صحیح ipi_mswi_cold_init(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_match *match)
{
	صحیح rc;
	unsigned long offset;
	ساختار aclint_mswi_data *ms;

	ms = sbi_zalloc(اندازه(*ms));
	اگر (!ms)
		بازگشت SBI_ENOMEM;

	rc = fdt_parse_aclint_گره(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, پوچ, پوچ,
				   &ms->first_hartid, &ms->hart_شمار);
	اگر (rc) {
		sbi_free(ms);
		بازگشت rc;
	}

	اگر (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		اگر ((ms->size - offset) < ACLINT_MSWI_SIZE)
			بازگشت SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	اگر (rc) {
		sbi_free(ms);
		بازگشت rc;
	}

	بازگشت 0;
}

ایستا ثابت unsigned long clint_offset = CLINT_MSWI_OFFSET;

ایستا ثابت ساختار fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت ساختار fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
