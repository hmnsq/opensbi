/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/ipi/aclint_mswi.h>

நிலையான முழு ipi_mswi_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	முழு rc;
	unsigned long offset;
	கட்டமைப்பு aclint_mswi_data *ms;

	ms = sbi_zalloc(அளவு(*ms));
	எனில் (!ms)
		திரும்பு SBI_ENOMEM;

	rc = fdt_parse_aclint_முனை(fdt, nodeoff, false, false,
				   &ms->addr, &ms->size, வெற்று, வெற்று,
				   &ms->first_hartid, &ms->hart_மொத்தம்);
	எனில் (rc) {
		sbi_free(ms);
		திரும்பு rc;
	}

	எனில் (match->data) {
		/* Adjust MSWI address and size for CLINT device */
		offset = *((unsigned long *)match->data);
		ms->addr += offset;
		எனில் ((ms->size - offset) < ACLINT_MSWI_SIZE)
			திரும்பு SBI_EINVAL;
		ms->size = ACLINT_MSWI_SIZE;
	}

	rc = aclint_mswi_cold_init(ms);
	எனில் (rc) {
		sbi_free(ms);
		திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான மாறிலி unsigned long clint_offset = CLINT_MSWI_OFFSET;

நிலையான மாறிலி கட்டமைப்பு fdt_match ipi_mswi_match[] = {
	{ .compatible = "riscv,clint0", .data = &clint_offset },
	{ .compatible = "sifive,clint0", .data = &clint_offset },
	{ .compatible = "thead,c900-clint", .data = &clint_offset },
	{ .compatible = "thead,c900-aclint-mswi" },
	{ .compatible = "mips,p8700-aclint-mswi" },
	{ .compatible = "riscv,aclint-mswi" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_ipi_mswi = {
	.match_table = ipi_mswi_match,
	.init = ipi_mswi_cold_init,
};
