/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_driver.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/ipi/aclint_mswi.h>

ساكن صحيح ipi_mswi_cold_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_match *match)
{
	صحيح rc;
	unsigned long offset;
	بنية aclint_mswi_data *ms;

	ms = sbi_zalloc(حجم(*ms));
	إذا (!ms)
		أرجع SBI_ENOMEM;

	rc = fdt_parse_aclint_نود(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, عدم, عدم,
				   &ms->first_hartid, &ms->hart_کيراءن);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	إذا (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		إذا ((ms->size - offset) < ACLINT_MSWI_SIZE)
			أرجع SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	إذا (rc) {
		sbi_free(ms);
		أرجع rc;
	}

	أرجع 0;
}

ساكن ثابت unsigned long clint_offset = CLINT_MSWI_OFFSET;

ساكن ثابت بنية fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

ثابت بنية fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
